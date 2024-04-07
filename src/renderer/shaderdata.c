#include <renderer/shaderdata.h>

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

shaderdata_t shaderdata_create()
{
  shaderdata_t sd = calloc(sizeof(*sd), 1);
  
  sd->num_vert = 0;
  sd->max_vert = 8;
  sd->vert = calloc(sizeof(*sd->vert), sd->max_frag);
  
  sd->num_frag = 0;
  sd->max_frag = 8;
  sd->frag = calloc(sizeof(*sd->frag), sd->max_frag);
  
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
  if (type & S_VERT) {
    if (sd->num_vert >= sd->max_vert) {
      sd->max_vert *= 2;
      sd->vert = realloc(sd->vert, sd->max_vert);
    }
    
    sd->vert[sd->num_vert++] = strdup(text);
  }
  
  if (type & S_FRAG) {
    if (sd->num_frag >= sd->max_frag) {
      sd->max_frag *= 2;
      sd->frag = realloc(sd->frag, sd->max_frag);
    }
    
    sd->frag[sd->num_frag++] = strdup(text);
  }
}

const char **shaderdata_get_vert(shaderdata_t sd, int *num_vert)
{
  *num_vert = sd->num_vert;
  return (const char**) sd->vert;
}

const char **shaderdata_get_frag(shaderdata_t sd, int *num_frag)
{
  *num_frag = sd->num_frag;
  return (const char**) sd->frag;
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
