#ifndef SHADERDATA_H
#define SHADERDATA_H

typedef enum {
  S_VERT = 1,
  S_FRAG = 2
} SHADERTYPE;

typedef struct shaderdata_s *shaderdata_t;

shaderdata_t shaderdata_create();
void shaderdata_text(shaderdata_t sd, const char *text, SHADERTYPE type);
void shaderdata_line(shaderdata_t sd, const char *text, SHADERTYPE type);
void shaderdata_source(shaderdata_t sd, const char *src, SHADERTYPE type);
const char **shaderdata_get_vert(shaderdata_t sd, int *num_vert);
const char **shaderdata_get_frag(shaderdata_t sd, int *num_frag);
void shaderdata_destroy(shaderdata_t sd);

#endif
