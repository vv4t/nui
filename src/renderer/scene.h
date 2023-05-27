#ifndef SCENE_H
#define SCENE_H

#include "gl.h"
#include "view.h"
#include "mesh.h"
#include "../common/nui_math.h"

typedef struct {
  GLuint  diffuse;
  GLuint  normal;
} material_t;

typedef struct {
  void    *data;
  view_t  *view;
  void    (*draw)(void *data, view_t *view);
} scene_t;

bool material_load(material_t *material, const char *src_diffuse, const char *src_normal);

void draw_scene(scene_t *scene); 
void draw_mesh(mesh_t mesh);

#endif
