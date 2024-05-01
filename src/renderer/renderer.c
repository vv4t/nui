#include <renderer/renderer.h>

#include <renderer/frame.h>
#include <renderer/texture.h>
#include <renderer/shader.h>
#include <renderer/mesh.h>
#include <renderer/target.h>
#include <renderer/camera.h>
#include <GL/glew.h>

#define MAX_VERTICES 1024

struct {
  mesh_t mesh;
  texture_t depth;
  texture_t texture;
  
  framebuffer_t fb[2];
  
  shader_t surface;
  shader_t hdr;
  shader_t dither;
} renderer;

void renderer_init()
{
  camera_init();
  vbuffer_init(MAX_VERTICES);
  vbuffer_bind();
  frame_init();
  
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
  
  float aspect_ratio = 800.0 / 600.0;
  camera_perspective(1.0, aspect_ratio, 1.0, 100.0);
  
  meshdata_t md = meshdata_create();
  vector cube_axis[][2] = {
    { vec3(+1, 0,  0), vec3(0, 1, 0) },
    { vec3(-1, 0,  0), vec3(0, 1, 0) },
    { vec3( 0, 0, +1), vec3(0, 1, 0) },
    { vec3( 0, 0, -1), vec3(0, 1, 0) }
  };
  for (int i = 0; i < sizeof(cube_axis) / sizeof(*cube_axis); i++) {
    vector x = cube_axis[i][0];
    vector y = cube_axis[i][1];
    vector z = cross(x, y);
    meshdata_add_quad(md, mdotm(translate(vec3(0, 0, -1)), mat3(x, y, z)));
  }
  renderer.mesh = vbuffer_add(md);
  meshdata_destroy(md);
  
  renderer.texture = texture_image("assets/image/test.png");
  renderer.depth = texture_create(400, 300, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT);
  
  renderer.fb[0] = framebuffer_create(400, 300, renderer.depth);
  renderer.fb[1] = framebuffer_create(400, 300, renderer.depth);
  framebuffer_bind(renderer.fb[0], 0, GL_TEXTURE_2D, framebuffer_get_texture(renderer.fb[1]));
  framebuffer_bind(renderer.fb[1], 0, GL_TEXTURE_2D, framebuffer_get_texture(renderer.fb[0]));
  
  renderer.hdr = frame_shader_load("assets/shader/frame/hdr.frag");
  renderer.dither = frame_shader_load("assets/shader/frame/dither.frag");
  
  shaderdata_t sd = shaderdata_create();
  camera_shader_import(sd);
  shaderdata_source(sd, "assets/shader/vertex/surface.vert", SD_VERT);
  shaderdata_source(sd, "assets/shader/surface/flat.frag", SD_FRAG);
  renderer.surface = shader_load(sd);
  camera_shader_attach(renderer.surface);
  shaderdata_destroy(sd);
}

float t = 0.0;

void renderer_render()
{
  t += 0.015;
  
  framebuffer_begin(renderer.fb[0]);
    shader_bind(renderer.surface);
    texture_bind(renderer.texture, GL_TEXTURE_2D, 0);
    camera_update(transform(vec3(0.0, 0.0, 4.0), vec3(0.0, t, 0.0), vec3(1.0, 1.0, 1.0)));
    vbuffer_draw(renderer.mesh);
  framebuffer_end();
  
  framebuffer_update(renderer.fb[1], renderer.hdr);
  
  glViewport(0, 0, 800, 600);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  framebuffer_draw(renderer.fb[0], renderer.dither);
}

void renderer_deinit()
{
  framebuffer_destroy(renderer.fb[0]);
  framebuffer_destroy(renderer.fb[1]);
  texture_destroy(renderer.texture);
  vbuffer_deinit();
  camera_deinit();
}
