#include <renderer/shader.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct shaderdata_s {
  char **vert;
  int num_vert;
  int max_vert;
  
  char **frag;
  int num_frag;
  int max_frag;
};

static GLuint shader_compile(GLuint type, const char **src, int num_src);

GLuint shader_load(shaderdata_t sd)
{
  GLuint vert_shader = shader_compile(GL_VERTEX_SHADER, (const char**) sd->vert, sd->num_vert);
  GLuint frag_shader = shader_compile(GL_FRAGMENT_SHADER, (const char**) sd->frag, sd->num_frag);
  
  GLuint shader = glCreateProgram();
  
  glAttachShader(shader, vert_shader);
  glAttachShader(shader, frag_shader);
  
  int success;
  static char info[1024];
  
  glLinkProgram(shader);
  glGetProgramiv(shader, GL_LINK_STATUS, &success);
  
  if (!success) {
    glGetProgramInfoLog(shader, sizeof(info), NULL, info);
    fprintf(stderr, "%s", info);
    exit(1);
  }
  
  glDetachShader(shader, vert_shader);
  glDetachShader(shader, frag_shader);
  glDeleteShader(vert_shader);
  glDeleteShader(frag_shader);
  
  return shader;
}

void shader_bind(shader_t shader)
{
  glUseProgram(shader);
}

void shader_destroy(shader_t shader)
{
  glDeleteProgram(shader);
}

GLuint shader_compile(GLuint type, const char **src, int num_src)
{
  int success;
  static GLchar info[1024];
  
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, num_src, src, NULL);
  
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  
  if (!success) {
    glGetShaderInfoLog(shader, sizeof(info), NULL, info);
    fprintf(stderr, "%s", info);
    exit(1);
  }
  
  return shader;
}

shaderdata_t shaderdata_create()
{
  shaderdata_t sd = calloc(sizeof(*sd), 1);
  
  sd->num_vert = 0;
  sd->max_vert = 8;
  sd->vert = calloc(sizeof(*sd->vert), sd->max_vert);
  
  sd->num_frag = 0;
  sd->max_frag = 8;
  sd->frag = calloc(sizeof(*sd->frag), sd->max_frag);
  
  shaderdata_line(sd, "#version 300 es", SD_VERT | SD_FRAG);
  shaderdata_line(sd, "precision mediump float;", SD_FRAG);
  
  return sd;
}

void shaderdata_source(shaderdata_t sd, const char *src, SHADERTYPE type)
{
  FILE *fp = fopen(src, "r");
  
  if (!fp) {
    perror(src);
    exit(1);
  }
  
  fseek(fp, 0, SEEK_END);
  size_t fsize = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  
  char *text = malloc(fsize + 1);
  fread(text, fsize, 1, fp);
  fclose(fp);
  
  text[fsize] = 0;
 
  shaderdata_text(sd, text, type);
  
  free(text);
}

void shaderdata_line(shaderdata_t sd, const char *text, SHADERTYPE type)
{
  shaderdata_text(sd, text, type);
  shaderdata_text(sd, "\n", type);
}

void shaderdata_text(shaderdata_t sd, const char *text, SHADERTYPE type)
{
  if (type & SD_VERT) {
    if (sd->num_vert >= sd->max_vert) {
      sd->max_vert *= 2;
      sd->vert = realloc(sd->vert, sd->max_vert * sizeof(char*));
    }
    
    sd->vert[sd->num_vert++] = strdup(text);
  }
  
  if (type & SD_FRAG) {
    if (sd->num_frag >= sd->max_frag) {
      sd->max_frag *= 2;
      sd->frag = realloc(sd->frag, sd->max_frag * sizeof(char*));
    }
    
    sd->frag[sd->num_frag++] = strdup(text);
  }
}

void shaderdata_destroy(shaderdata_t sd)
{
  for (int i = 0; i < sd->num_vert; i++) {
    free(sd->vert[i]);
  }
  
  for (int i = 0; i < sd->num_frag; i++) {
    free(sd->frag[i]);
  }
  
  free(sd->vert);
  free(sd->frag);
  free(sd);
}
