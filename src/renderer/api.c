#include "api.h"

#include "camera.h"
#include "material.h"
#include "../gl/shader.h"
#include "../common/path.h"
#include "../common/file.h"

static const char *glsl_version = "#version 300 es";
static const char *glsl_precision = "precision mediump float;";

bool forward_shader_load(GLuint *shader, const char *name)
{
  path_t path_vert;
  path_t path_frag;
  
  path_create(path_vert, "assets/shader/%s/%s.vsh", name, name);
  path_create(path_frag, "assets/shader/%s/%s.fsh", name, name);
  
  char *vsh = file_read_all(path_vert);
  
  if (!vsh) {
    return false;
  }
  
  char *fsh = file_read_all(path_frag);
  
  if (!fsh) {
    return false;
  }
  
  const char *vsh_src[] = {
    glsl_version, "\n",
    glsl_precision, "\n",
    "\n\
layout(location = 0) in vec3 v_pos;\n\
layout(location = 1) in vec3 v_tangent;\n\
layout(location = 2) in vec3 v_bitangent;\n\
layout(location = 3) in vec3 v_normal;\n\
layout(location = 4) in vec2 v_uv;\n\
\n\
out vec2 vs_uv;\n\
out vec3 vs_normal;\n\
out vec3 vs_pos;",
    camera_shader_ext(), "\n",
    vsh, "\n",
    "\n\
\n\
void main()\n\
{\n\
  vs_uv = v_uv;\n\
  vs_normal = v_normal;\n\
  vs_pos = vec3(get_model() * vec4(v_pos, 1.0));\n\
  gl_Position = get_mvp() * vec4(v_pos, 1.0);\n\
  vertex_pass();\n\
}",
  NULL
  };
  
  const char *fsh_src[] = {
    glsl_version, "\n",
    glsl_precision, "\n",
    "\n\
out vec4 frag_color;\n\
\n\
in vec3 vs_pos;\n\
in vec3 vs_normal;\n\
in vec2 vs_uv;\n\
\n\
vec3 get_frag_pos() { return vs_pos; }\n\
vec3 get_frag_normal() { return vs_normal; }\n\
void set_frag(vec4 v) { frag_color = v; }",
    camera_shader_ext(), "\n",
    material_shader_ext(), "\n",
    fsh, "\n",
    "void main() { frag_pass(); }",
    NULL
  };
  
  if (!shader_load(shader, name, vsh_src, fsh_src)) {
    return false;
  }
  
  material_shader_ext_setup(*shader);
  camera_shader_ext_setup(*shader);
  
  free(fsh);
  free(vsh);
  
  return true;
}

bool fx_shader_load(GLuint *shader, const char *name)
{
  const char *vert_src[] = {
    glsl_version, "\n",
    glsl_precision, "\n",
    "\n\
layout(location = 0) in vec3 v_pos;\n\
layout(location = 4) in vec2 v_uv;\n\
\n\
out vec2 vs_uv;\n\
\n\
void main()\n\
{\n\
  vs_uv = v_uv;\n\
  gl_Position = vec4(v_pos, 1.0);\n\
}",
  NULL
  };
  
  path_t path_fx;
  path_create(path_fx, "assets/shader/fx/%s.fsh", name, name);
  
  char *fx_src = file_read_all(path_fx);
  
  if (!fx_src) {
    return false;
  }
  
  const char *frag_src[] = {
    glsl_version, "\n",
    glsl_precision, "\n",
    "\n\
out vec4 frag_color;\n\
in vec2 vs_uv;\n\
uniform sampler2D u_scene;\n\
void set_frag(vec4 v) { frag_color = v; }\n\
vec4 get_color(vec2 uv) { return texture(u_scene, uv); }\n\
vec2 get_uv() { return vs_uv; }",
    fx_src, "\n",
    "void main() { frag_pass(); }",
    NULL
  };
  
  if (!shader_load(shader, name, vert_src, frag_src)) {
    return false;
  }
  
  free(fx_src);
  
  return true;
}

bool defer_shader_load(GLuint *shader, const char *name)
{
  const char *vert_src[] = {
    glsl_version, "\n",
    glsl_precision, "\n",
    camera_shader_ext(),
    "\n\
layout(location = 0) in vec3 v_pos;\n\
layout(location = 4) in vec2 v_uv;\n\
\n\
out vec2 vs_uv;\n\
out vec3 vs_ray;\n\
\n\
void main()\n\
{\n\
  vs_uv = v_uv;\n\
  vs_ray = vec3(inverse(mat_look) * vec4(v_pos, 1.0));\n\
  gl_Position = vec4(v_pos, 1.0);\n\
}",
  NULL
  };
  
  path_t path_defer;
  path_create(path_defer, "assets/shader/defer/%s.fsh", name, name);
  
  char *defer_src = file_read_all(path_defer);
  
  if (!defer_src) {
    return false;
  }
  
  const char *frag_src[] = {
    glsl_version, "\n",
    glsl_precision, "\n",
    camera_shader_ext(),
    "\n\
out vec4 frag_color;\n\
\n\
in vec2 vs_uv;\n\
in vec3 vs_ray;\n\
\n\
uniform sampler2D u_pos;\n\
uniform sampler2D u_normal;\n\
uniform sampler2D u_albedo;\n\
\n\
vec3 get_frag_pos() { return texture(u_pos, vs_uv).rgb; }\n\
vec3 get_world_pos() { return normalize(vs_ray) * texture(u_normal, vs_uv).w + view_pos; }\n\
vec3 get_frag_normal() { return texture(u_normal, vs_uv).rgb; }\n\
vec4 get_diffuse() { return texture(u_albedo, vs_uv); }\n\
void set_frag(vec4 v) { frag_color = v; }\n",
    defer_src,
    "void main() { frag_pass(); }",
    NULL
  };
  
  if (!shader_load(shader, name, vert_src, frag_src)) {
    return false;
  }
  
  glUseProgram(*shader);
  
  GLuint ul_pos = glGetUniformLocation(*shader, "u_pos");
  GLuint ul_normal = glGetUniformLocation(*shader, "u_normal");
  GLuint ul_albedo = glGetUniformLocation(*shader, "u_albedo");
  
  glUniform1i(ul_pos, 0);
  glUniform1i(ul_normal, 1);
  glUniform1i(ul_albedo, 2);
  
  return true;
}

bool custom_shader_load(GLuint *shader, const char *name, const char *ext[])
{
  path_t path_vert;
  path_t path_frag;
  
  path_create(path_vert, "assets/shader/%s/%s.vsh", name, name);
  path_create(path_frag, "assets/shader/%s/%s.fsh", name, name);
  
  char *vsh = file_read_all(path_vert);
  
  if (!vsh) {
    return false;
  }
  
  char *fsh = file_read_all(path_frag);
  
  if (!fsh) {
    return false;
  }
  
  const char *vsh_src[256];
  const char *fsh_src[256];
  
  int num_src = 0;
  
  if (ext) {
    for (num_src = 0; ext[num_src]; num_src++) {
      vsh_src[4 + num_src] = ext[num_src];
      fsh_src[4 + num_src] = ext[num_src];
    }
  }
  
  vsh_src[0] = glsl_version;
  vsh_src[1] = "\n";
  vsh_src[2] = glsl_precision;
  vsh_src[3] = "\n";
  vsh_src[4 + num_src + 0] = vsh;
  vsh_src[4 + num_src + 1] = NULL;
  
  fsh_src[0] = glsl_version;
  fsh_src[1] = "\n";
  fsh_src[2] = glsl_precision;
  fsh_src[3] = "\n";
  fsh_src[4 + num_src + 0] = fsh;
  fsh_src[4 + num_src + 1] = NULL;
  
  if (!shader_load(shader, name, vsh_src, fsh_src)) {
    return false;
  }
  
  return true;
}
