#include <renderer/camera.h>

#include <GL/glew.h>

struct ub_camera {
  matrix model;
  matrix mvp;
};

struct {
  matrix p;
  matrix v;
  GLuint ubo;
} camera;

void camera_init()
{
  glGenBuffers(1, &camera.ubo);
  glBindBuffer(GL_UNIFORM_BUFFER, camera.ubo);
  glBufferData(GL_UNIFORM_BUFFER, 512, NULL, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, camera.ubo);
  
  camera.v = identity();
}

void camera_shader_attach(shader_t shader)
{
  shader_bind(shader);
  glUniformBlockBinding(shader, glGetUniformBlockIndex(shader, "ub_camera"), 0);
}

void camera_shader_import(shaderdata_t sd)
{
  shaderdata_source(sd, "assets/shader/import/camera.glsl", SD_VERT | SD_FRAG);
}

void camera_orthogonal(float w, float h)
{
  camera.p = scale(vec3(w, h, 1.0));
}

void camera_perspective(float w, float h, float n, float f)
{
  camera.p = perspective(w, h, n, f);
}

void camera_move(vector position, vector rotation)
{
  vector view_offset = fdotv(-1.0, position);
  vector view_angle = fdotv(-1.0, rotation);
  camera.v = mdotm(translate(view_offset), rotate_zyx(view_angle));
}

void camera_update(matrix model)
{
  struct ub_camera ub_camera = {
    .model = model,
    .mvp = mdotm(model, mdotm(camera.v, camera.p))
  };
  
  glBindBuffer(GL_UNIFORM_BUFFER, camera.ubo);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ub_camera), &ub_camera);
}

void camera_deinit()
{
  glDeleteBuffers(1, &camera.ubo);
}
