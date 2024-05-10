#include <renderer/skybox.h>
#include <renderer/shader.h>
#include <renderer/camera.h>
#include <renderer/mesh.h>
#include <GL/glew.h>

#include <renderer/light.h>

struct {
  shader_t shader;
  mesh_t mesh;
  texture_t cube_map;
} skybox;

void skybox_init()
{
  meshdata_t md = meshdata_create();
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
      
      meshdata_add_quad(md, mdotm(translate(vec3(0, 0, 1)), mat3(cross(y, z), y, z)));
    }
    skybox.mesh = vbuffer_add(md);
  meshdata_destroy(md);
  
  shaderdata_t sd = shaderdata_create();
  camera_shader_import(sd);
  light_shader_import(sd);
  shaderdata_source(sd, "assets/shader/skybox/skybox.vert", SD_VERT);
  shaderdata_source(sd, "assets/shader/skybox/skybox.frag", SD_FRAG);
  skybox.shader = shader_load(sd);
  camera_shader_attach(skybox.shader);
  light_shader_attach(skybox.shader);
  shaderdata_destroy(sd);
}

void skybox_set_cube_map(texture_t cube_map)
{
  skybox.cube_map = cube_map;
}

void skybox_draw()
{
  glDepthMask(GL_FALSE);
  shader_bind(skybox.shader);
  texture_bind(skybox.cube_map, GL_TEXTURE_CUBE_MAP, 0);
  camera_update(identity());
  vbuffer_draw(skybox.mesh);
  glDepthMask(GL_TRUE);
}

void skybox_deinit()
{
  shader_destroy(skybox.shader);
}
