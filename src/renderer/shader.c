#include "gl.h"

#define INFO_MAX 1024

#include "../common/path.h"
#include "../common/file.h"
#include "../common/log.h"

typedef struct {
  const char *src;
  const char *head;
  const char *tail;
} cfg_shader_t;

static bool shader_compile(GLuint *shader, cfg_shader_t *cfg_shader, GLuint type);
static bool shader_load_each(GLuint *shader, const char *name, cfg_shader_t *cfg_vertex, cfg_shader_t *cfg_fragment);

const char *glsl_version = "#version 300 es";
const char *glsl_precision = "precision mediump float;";

bool shader_load(GLuint *shader, const char *name, const char *define)
{
  path_t path_vertex;
  path_t path_fragment;
  
  path_create(path_vertex, "assets/shader/%s/%s.vsh", name, name);
  path_create(path_fragment, "assets/shader/%s/%s.fsh", name, name);
  
  cfg_shader_t cfg_vsh;
  cfg_vsh.src = path_vertex;
  cfg_vsh.head = "";
  cfg_vsh.tail = "";
  
  cfg_shader_t cfg_fsh;
  cfg_fsh.src = path_fragment;
  cfg_fsh.head = "";
  cfg_fsh.tail = "";
  
  if (!shader_load_each(shader, name, &cfg_vsh, &cfg_fsh)) {
    return false;
  }
  
  glUseProgram(*shader);
  
  GLuint ul_color = glGetUniformLocation(*shader, "u_color");
  glUniform1i(ul_color, 0);
  
  GLuint ubl_camera = glGetUniformBlockIndex(*shader, "ub_camera");
  glUniformBlockBinding(*shader, ubl_camera, 0);
  
  GLuint ubl_material = glGetUniformBlockIndex(*shader, "ub_material");
  glUniformBlockBinding(*shader, ubl_material, 1);
  
  return true;
}

bool forward_shader_load(GLuint *shader, const char *name)
{
  const char *vertex_head = "\n\
layout(location = 0) in vec3 v_pos;\n\
layout(location = 1) in vec3 v_tangent;\n\
layout(location = 2) in vec3 v_bitangent;\n\
layout(location = 3) in vec3 v_normal;\n\
layout(location = 4) in vec2 v_uv;\n\
\n\
mat4 get_mvp() { return mat_mvp; }\n\
mat4 get_model() { return mat_model; }\n\
mat4 get_view() { return mat_view; }\n\
vec3 get_view_pos() { return view_pos; }\n\
\n\
out vec2 vs_uv;\n\
out vec3 vs_normal;\n\
out vec3 vs_pos;";
  
  const char *vertex_tail = "\n\
void main()\n\
{\n\
  vs_uv = v_uv;\n\
  vs_normal = v_normal;\n\
  vs_pos = vec3(get_model() * vec4(v_pos, 1.0));\n\
  gl_Position = get_mvp() * vec4(v_pos, 1.0);\n\
  vertex_pass();\n\
}";
  
  const char *frag_head = "\n\
out vec4 frag_color;\n\
\n\
in vec3 vs_pos;\n\
in vec3 vs_normal;\n\
in vec2 vs_uv;\n\
\n\
uniform sampler2D u_color;\n\
\n\
vec3 get_frag_pos() { return vs_pos; }\n\
vec3 get_frag_normal() { return vs_normal; }\n\
vec4 get_diffuse() { return texture(u_color, vs_uv) * vec4(m_color, 1.0); }\n\
void set_frag(vec4 v) { frag_color = v; }";
  
  const char *frag_tail = "void main() { frag_pass(); }";
  
  path_t path_vertex;
  path_t path_fragment;
  
  path_create(path_vertex, "assets/shader/%s/%s.vsh", name, name);
  path_create(path_fragment, "assets/shader/%s/%s.fsh", name, name);
  
  cfg_shader_t cfg_vsh;
  cfg_vsh.src = path_vertex;
  cfg_vsh.head = vertex_head;
  cfg_vsh.tail = vertex_tail;
  
  cfg_shader_t cfg_fsh;
  cfg_fsh.src = path_fragment;
  cfg_fsh.head = frag_head;
  cfg_fsh.tail = frag_tail;
  
  if (!shader_load_each(shader, name, &cfg_vsh, &cfg_fsh)) {
    return false;
  }
  
  glUseProgram(*shader);
  
  GLuint ul_color = glGetUniformLocation(*shader, "u_color");
  glUniform1i(ul_color, 0);
  
  GLuint ubl_camera = glGetUniformBlockIndex(*shader, "ub_camera");
  glUniformBlockBinding(*shader, ubl_camera, 0);
  
  GLuint ubl_material = glGetUniformBlockIndex(*shader, "ub_material");
  glUniformBlockBinding(*shader, ubl_material, 1);
  
  return true;
}

bool fx_shader_load(GLuint *shader, const char *name, const char *define)
{
  const char *fx_vertex = "\n\
layout(location = 0) in vec3 v_pos;\n\
layout(location = 4) in vec2 v_uv;\n\
\n\
out vec2 vs_uv;\n\
\n\
void main()\n\
{\n\
  vs_uv = v_uv;\n\
  gl_Position = vec4(v_pos, 1.0);\n\
}";
  
  const char *frag_head = "\n\
out vec4 frag_color;\n\
in vec2 vs_uv;\n\
uniform sampler2D u_scene;\n\
void set_frag(vec4 v) { frag_color = v; }\n\
vec4 get_color(vec2 uv) { return texture(u_scene, uv); }\n\
vec2 get_uv() { return vs_uv; }";
  
  const char *frag_tail = "void main() { frag_pass(); }";
  
  path_t path_fx;
  path_create(path_fx, "assets/shader/fx/%s.fsh", name);
  
  cfg_shader_t cfg_vsh;
  cfg_vsh.src = NULL;
  cfg_vsh.head = fx_vertex;
  cfg_vsh.tail = "";
  
  cfg_shader_t cfg_fsh;
  cfg_fsh.src = path_fx;
  cfg_fsh.head = frag_head;
  cfg_fsh.tail = frag_tail;
  
  return shader_load_each(shader, name, &cfg_vsh, &cfg_fsh);
}

bool defer_shader_load(GLuint *shader, const char *name, const char *define)
{
  const char *defer_vertex = "\n\
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
}";
  
  const char *frag_head = "\n\
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
void set_frag(vec4 v) { frag_color = v; }";

  const char *frag_tail = "void main() { frag_pass(); }";

  path_t path_defer;
  path_create(path_defer, "assets/shader/defer/%s.fsh", name);
  
  cfg_shader_t cfg_vsh;
  cfg_vsh.src = NULL;
  cfg_vsh.head = defer_vertex;
  cfg_vsh.tail = "";
  
  cfg_shader_t cfg_fsh;
  cfg_fsh.src = path_defer;
  cfg_fsh.head = frag_head;
  cfg_fsh.tail = frag_tail;
  
  if (!shader_load_each(shader, name, &cfg_vsh, &cfg_fsh)) {
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

static bool shader_load_each(GLuint *shader, const char *name, cfg_shader_t *cfg_vertex, cfg_shader_t *cfg_fragment)
{
  *shader = glCreateProgram();
  
  GLuint vertex_shader;
  if (!shader_compile(&vertex_shader, cfg_vertex, GL_VERTEX_SHADER)) {
    return false;
  }
  
  GLuint fragment_shader;
  if (!shader_compile(&fragment_shader, cfg_fragment, GL_FRAGMENT_SHADER)) {
    return false;
  }
  
  glAttachShader(*shader, vertex_shader);
  glAttachShader(*shader, fragment_shader);
  
  int success;
  static char info[INFO_MAX];
  
  glLinkProgram(*shader);
  glGetProgramiv(*shader, GL_LINK_STATUS, &success);
  
  if (!success) {
    glGetProgramInfoLog(*shader, INFO_MAX, NULL, info);
    LOG_ERROR("failed to link shader (%s):\n%s", name, info);
    return false;
  }
  
  glDetachShader(*shader, vertex_shader);
  glDeleteShader(vertex_shader);

  glDetachShader(*shader, fragment_shader);
  glDeleteShader(fragment_shader);
  
  return true;
}

static bool shader_compile(GLuint *shader, cfg_shader_t *cfg_shader, GLuint type)
{
  char *src = "";
  
  if (cfg_shader->src) {
    src = file_read_all(cfg_shader->src);
    
    if (!src) {
      return false;
    }
  }
  
  int success;
  static GLchar info[INFO_MAX];
  
  const char *ub_camera = "\
layout (std140) uniform ub_camera {\n\
  mat4 mat_mvp;\n\
  mat4 mat_model;\n\
  mat4 mat_view;\n\
  mat4 mat_look;\n\
  vec3 view_pos;\n\
};";

  const char *ub_material = "\
layout (std140) uniform ub_material {\n\
  vec3 m_color;\n\
  float m_specular;\n\
};";
  
  const char *full_src[] = {
    glsl_version, "\n",
    glsl_precision, "\n",
    ub_camera, "\n",
    ub_material, "\n",
    cfg_shader->head, "\n",
    src, "\n",
    cfg_shader->tail
  };
  
  *shader = glCreateShader(type);
  glShaderSource(*shader, sizeof(full_src) / sizeof(char*), full_src, NULL);
  
  glCompileShader(*shader);
  glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);
  
  if (!success) {
    glGetShaderInfoLog(*shader, INFO_MAX, NULL, info);
    LOG_ERROR("failed to compiler shader (%s):\n\n%s", cfg_shader->src ? cfg_shader->src : "BUILTIN", info);
    return false;
  }
  
  if (cfg_shader->src) {
    free(src);
  }
  
  return true;
}
