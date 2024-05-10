#ifndef MESHINSTANCE_H
#define MESHINSTANCE_H

typedef enum {
  MESH_PLANE,
  MESH_CUBE,
  MAX_MESHNAME
} meshname_t;

typedef struct {
  meshname_t meshname;
} meshinstance_t;

inline meshinstance_t create_meshinstance()
{
  return (meshinstance_t) {0};
}

#endif