#include <renderer/renderer.h>

#include <renderer/frame.h>
#include <renderer/texture.h>
#include <renderer/shader.h>
#include <renderer/mesh.h>
#include <renderer/target.h>
#include <renderer/camera.h>
#include <GL/glew.h>

#define SCR_WIDTH 800
#define SCR_HEIGHT 600

#define VIEW_SCALE 4
#define VIEW_WIDTH (SCR_WIDTH / VIEW_SCALE)
#define VIEW_HEIGHT (SCR_HEIGHT / VIEW_SCALE)

#define MAX_VERTICES 1024

struct {
  mesh_t meshname[MAX_MESHNAME];
  
  frame_t buffer[2];
  texture_t depth;
  texture_t texture;
  
  shader_t surface;
  shader_t hdr;
  shader_t dither;
} renderer;

static void renderer_init_asset();
static void renderer_init_buffer();
static void renderer_init_surface();

static void renderer_draw_entities(const game_t *gs);

void renderer_init()
{
  camera_init();
  vbuffer_init(MAX_VERTICES);
  vbuffer_bind();
  frame_init();
  
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  
  renderer_init_asset();
  renderer_init_surface();
  renderer_init_buffer();
  
  float aspect_ratio = (float) SCR_WIDTH / (float) SCR_HEIGHT;
  camera_perspective(1.0, aspect_ratio, 0.1, 100.0);
}

float t = 0.0;

void renderer_render(const game_t *gs)
{
  t += 0.015;
  
  const transform_t *pt = ENTITY_GET_COMPONENT(gs->edict, gs->player, transform);
  camera_move(pt->position, pt->rotation);
  
  frame_begin(renderer.buffer[0]);
  shader_bind(renderer.surface);
  texture_bind(renderer.texture, GL_TEXTURE_2D, 0);
  renderer_draw_entities(gs);
  frame_end();
  
  frame_update(renderer.buffer[1], renderer.hdr);
  
  glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  frame_draw(renderer.buffer[0], renderer.dither);
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
    vbuffer_draw(renderer.meshname[m->meshname]);
  }
}

void renderer_init_surface()
{
  shaderdata_t sd = shaderdata_create();
  camera_shader_import(sd);
  shaderdata_source(sd, "assets/shader/vertex/surface.vert", SD_VERT);
  shaderdata_source(sd, "assets/shader/surface/light.frag", SD_FRAG);
  renderer.surface = shader_load(sd);
  camera_shader_attach(renderer.surface);
  shaderdata_destroy(sd);
}

void renderer_init_asset()
{
  meshdata_t md = meshdata_create();
    meshdata_add_quad(md, rotate_x(-M_PI / 2.0));
    meshdata_add_quad(md, rotate_x(+M_PI / 2.0));
    renderer.meshname[MESH_PLANE] = vbuffer_add(md);
  meshdata_destroy(md);
  
  md = meshdata_create();
    vector cube_axis[][2] = {
      { vec3(+1,  0,  0), vec3( 0,  1,  0) },
      { vec3(-1,  0,  0), vec3( 0,  1,  0) },
      { vec3( 0,  0, +1), vec3( 0,  1,  0) },
      { vec3( 0,  0, -1), vec3( 0,  1,  0) },
      { vec3( 0, +1,  0), vec3( 0,  0, -1) },
      { vec3( 0, -1,  0), vec3( 0,  0,  1) }
    };
    for (int i = 0; i < sizeof(cube_axis) / sizeof(*cube_axis); i++) {
      vector x = cube_axis[i][0];
      vector y = cube_axis[i][1];
      meshdata_add_quad(md, mdotm(translate(vec3(0, 0, -1)), mat3(x, y, cross(x, y))));
    }
    renderer.meshname[MESH_CUBE] = vbuffer_add(md);
  meshdata_destroy(md);
  
  renderer.texture = texture_image("assets/image/test.png");
}

void renderer_init_buffer()
{
  renderer.depth = texture_create(VIEW_WIDTH, VIEW_HEIGHT, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT);
  
  renderer.buffer[0] = frame_create(VIEW_WIDTH, VIEW_HEIGHT, renderer.depth);
  renderer.buffer[1] = frame_create(VIEW_WIDTH, VIEW_HEIGHT, renderer.depth);
  frame_bind(renderer.buffer[0], 0, GL_TEXTURE_2D, frame_get_texture(renderer.buffer[1]));
  frame_bind(renderer.buffer[1], 0, GL_TEXTURE_2D, frame_get_texture(renderer.buffer[0]));
  
  renderer.hdr = frame_shader_load("assets/shader/frame/hdr.frag");
  renderer.dither = frame_shader_load("assets/shader/frame/dither.frag");
}

void renderer_deinit()
{
  frame_destroy(renderer.buffer[0]);
  frame_destroy(renderer.buffer[1]);
  texture_destroy(renderer.texture);
  vbuffer_deinit();
  camera_deinit();
}
