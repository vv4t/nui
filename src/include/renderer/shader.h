#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>

typedef enum {
  SD_VERT = 1,
  SD_FRAG = 2
} SHADERTYPE;

typedef struct shaderdata_s *shaderdata_t;

shaderdata_t shaderdata_create();
void shaderdata_text(shaderdata_t sd, const char *text, SHADERTYPE type);
void shaderdata_line(shaderdata_t sd, const char *text, SHADERTYPE type);
void shaderdata_source(shaderdata_t sd, const char *src, SHADERTYPE type);
void shaderdata_destroy(shaderdata_t sd);

typedef GLuint shader_t;

shader_t shader_load(shaderdata_t sd);
void shader_bind(shader_t shader);
void shader_destroy(shader_t shader);

#endif
