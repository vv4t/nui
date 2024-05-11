#include <renderer/renderer.h>
#include <renderer/skybox.h>
#include <renderer/surface.h>
#include <renderer/frame.h>
#include <renderer/texture.h>
#include <renderer/shader.h>
#include <renderer/mesh.h>
#include <renderer/target.h>
#include <renderer/camera.h>
#include <renderer/light.h>
#include <GL/glew.h>
#include <stdio.h>

#define SCR_WIDTH 800
#define SCR_HEIGHT 600

#define VIEW_SCALE 4
#define VIEW_WIDTH (SCR_WIDTH / VIEW_SCALE)
#define VIEW_HEIGHT (SCR_HEIGHT / VIEW_SCALE)

#define MAX_VERTICES 1024

struct {
  mesh_t meshname[MAX_MESHNAME];
  material_t matname[MAX_MATNAME];
  
  frame_t frame[2];
  texture_t depth;
  
  texture_t albedo;
  texture_t position;
  texture_t normal;
  
  target_t t_gbuffer;
  target_t t_deferred;
  
  texture_t sky_star;
  
  surface_t sf_1;
  surface_t sf_gbuffer;
  
  shader_t hdr;
  shader_t dither;
  shader_t deferred;
} renderer;

static void renderer_init_assets();
static void renderer_init_frame();
static void renderer_init_surface();

static void renderer_draw_entities(const game_t *gs);

void renderer_init()
{
  camera_init();
  vbuffer_init(MAX_VERTICES);
  vbuffer_bind();
  frame_init();
  light_init();
  skybox_init();
  
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  
  renderer_init_assets();
  renderer_init_surface();
  renderer_init_frame();
  
  renderer.albedo = texture_create(VIEW_WIDTH, VIEW_HEIGHT, GL_RGBA, GL_FLOAT);
  renderer.position = texture_create(VIEW_WIDTH, VIEW_HEIGHT, GL_RGBA, GL_FLOAT);
  renderer.normal = texture_create(VIEW_WIDTH, VIEW_HEIGHT, GL_RGBA, GL_FLOAT);
  
  renderer.t_gbuffer = target_create(
    4,
    GL_COLOR_ATTACHMENT0, renderer.albedo,
    GL_COLOR_ATTACHMENT1, renderer.position,
    GL_COLOR_ATTACHMENT2, renderer.normal,
    GL_DEPTH_ATTACHMENT, renderer.depth
  );
  
  light_add_point(vec3(0.0, 1.0, 0.0), vec4(1.0, 1.0, 1.0, 5.0));
  skybox_set_cube_map(renderer.sky_star);
  
  float aspect_ratio = (float) SCR_WIDTH / (float) SCR_HEIGHT;
  camera_perspective(1.0, aspect_ratio, 0.1, 100.0);
}

float t = 0.0;

void renderer_render(const game_t *gs)
{
  t += 0.015;
  
  const transform_t *pt = ENTITY_GET_COMPONENT(gs->edict, gs->player, transform);
  camera_move(pt->position, pt->rotation);
  
  target_bind(renderer.t_gbuffer);
    glViewport(0, 0, VIEW_WIDTH, VIEW_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    skybox_draw();
    surface_use(renderer.sf_gbuffer);
    renderer_draw_entities(gs);
  target_unbind();
  
  glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  texture_bind(renderer.albedo, GL_TEXTURE_2D, 0);
  texture_bind(renderer.position, GL_TEXTURE_2D, 1);
  texture_bind(renderer.normal, GL_TEXTURE_2D, 2);
  frame_draw_mesh(renderer.deferred);
}

void renderer_draw_entities(const game_t *gs)
{
  for (entity_t e = 0; e < gs->edict.num_entities; e++) {
    if (ENTITY_MATCH(gs->edict, e, C_transform | C_meshinstance)) {
      continue;
    }
    
    const transform_t *t = ENTITY_GET_COMPONENT(gs->edict, e, transform);
    const meshinstance_t *m = ENTITY_GET_COMPONENT(gs->edict, e, meshinstance);
    
    camera_update(transform(t->position, t->rotation, t->scale));
    material_bind(renderer.matname[m->matname]);
    vbuffer_draw(renderer.meshname[m->meshname]);
  }
}

void renderer_init_surface()
{
  shaderdata_t sd = shaderdata_create();
    renderer.sf_1 = surface_create();
    surface_bind(renderer.sf_1, "u_cubemap", GL_TEXTURE_CUBE_MAP, renderer.sky_star);
    light_shader_import(sd);
    surface_shader_load(renderer.sf_1, sd, "assets/shader/surface/1.frag");
    light_shader_attach(surface_get_shader(renderer.sf_1));
  shaderdata_destroy(sd);
  
  sd = shaderdata_create();
    renderer.sf_gbuffer = surface_create();
    surface_shader_load(renderer.sf_gbuffer, sd, "assets/shader/surface/gbuffer.frag");
  shaderdata_destroy(sd);
  
  /*
  renderer.sf_gbuffer = surface_create();
  renderer.sh_gbuffer = surface_shader_load(renderer.sf_gbuffer, "assets/shader/surface/gbuffer.frag");
  
  shaderdata_t sd = shaderdata_create();
    light_shader_import(sd);
    surface_shader_source(sd, "assets/shader/surface/1.frag");
    renderer.sh_1 = shader_load(sd);
    surface_shader_attach(renderer.sh_1, renderer.sf_forward);
    light_shader_attach(renderer.sh_1);
  shaderdata_destroy(sd);
  */
}

void renderer_init_assets()
{
  meshdata_t md = meshdata_create();
    meshdata_add_quad(md, rotate_x(+M_PI / 2.0));
    renderer.meshname[MESH_PLANE] = vbuffer_add(md);
  meshdata_destroy(md);
  
  md = meshdata_create();
    vector cube_axis[][2] = {
      { vec3( 0, -1,  0), vec3(-1,  0,  0) },
      { vec3( 0, +1,  0), vec3(+1,  0,  0) },
      { vec3( 0, -1,  0), vec3( 0,  0, -1) },
      { vec3( 0, +1,  0), vec3( 0,  0, +1) },
      { vec3( 0,  0, -1), vec3( 0, -1,  0) },
      { vec3( 0,  0, -1), vec3( 0, +1,  0) }
    };
    for (int i = 0; i < sizeof(cube_axis) / sizeof(*cube_axis); i++) {
      vector y = cube_axis[i][0];
      vector z = cube_axis[i][1];
      
      meshdata_add_quad(md, mdotm(translate(vec3(0, 0, -1)), mat3(cross(y, z), y, z)));
    }
    renderer.meshname[MESH_CUBE] = vbuffer_add(md);
  meshdata_destroy(md);
  
  renderer.matname[MAT_TILE] = material_create(
    texture_load_image("assets/mat/tile/albedo.jpg"),
    texture_load_image("assets/mat/tile/normal.jpg")
  );
  
  renderer.matname[MAT_BRICK] = material_create(
    texture_load_image("assets/mat/brick/albedo.jpg"),
    texture_load_image("assets/mat/brick/normal.jpg")
  );
  
  renderer.matname[MAT_GRASS] = material_create(
    texture_load_image("assets/mat/grass/albedo.jpg"),
    texture_load_image("assets/mat/grass/normal.jpg")
  );
  
  renderer.sky_star = texture_load_cubemap("assets/skybox/star", "jpg");
}

void renderer_init_frame()
{
  renderer.depth = texture_create(VIEW_WIDTH, VIEW_HEIGHT, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT);
  
  renderer.frame[0] = frame_create(VIEW_WIDTH, VIEW_HEIGHT, renderer.depth);
  renderer.frame[1] = frame_create(VIEW_WIDTH, VIEW_HEIGHT, renderer.depth);
  frame_bind(renderer.frame[0], 0, GL_TEXTURE_2D, frame_get_texture(renderer.frame[1]));
  frame_bind(renderer.frame[1], 0, GL_TEXTURE_2D, frame_get_texture(renderer.frame[0]));
  
  renderer.hdr = frame_shader_load("assets/shader/frame/hdr.frag");
  renderer.dither = frame_shader_load("assets/shader/frame/dither.frag");
  renderer.deferred = frame_shader_load("assets/shader/frame/deferred.frag");
  
  shader_bind(renderer.deferred);
  glUniform1i(glGetUniformLocation(renderer.deferred, "u_albedo"), 0); 
  glUniform1i(glGetUniformLocation(renderer.deferred, "u_position"), 1); 
  glUniform1i(glGetUniformLocation(renderer.deferred, "u_normal"), 2);
}

void renderer_deinit()
{
  for (int i = 0; i < MAX_MATNAME; i++) {
    texture_destroy(renderer.matname[i].albedo);
    texture_destroy(renderer.matname[i].normal);
  }
  
  surface_destroy(renderer.sf_1);
  frame_destroy(renderer.frame[0]);
  frame_destroy(renderer.frame[1]);
  skybox_deinit();
  light_deinit();
  vbuffer_deinit();
  camera_deinit();
}
