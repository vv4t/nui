#include <renderer/camera.h>

#include <GL/glew.h>

struct ub_camera {
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
  
  GLuint ubl_camera = glGetUniformBlockIndex(shader, "ub_camera");
  glUniformBlockBinding(shader, ubl_camera, 0);
}

void camera_shader_import(shaderdata_t sd)
{
  shaderdata_source(sd, "assets/shader/import/camera.glsl", S_VERT | S_FRAG);
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
  camera.v = transform(fdotv(-1.0, position), fdotv(-1.0, rotation), vec3(1.0, 1.0, 1.0));
}

void camera_update(matrix m)
{
  struct ub_camera ub_camera = {
    .mvp = mdotm(m, mdotm(camera.v, camera.p))
  };
  
  glBindBuffer(GL_UNIFORM_BUFFER, camera.ubo);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ub_camera), &ub_camera);
}

void camera_deinit()
{
  glDeleteBuffers(1, &camera.ubo);
}
