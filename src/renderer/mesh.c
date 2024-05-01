#include <renderer/mesh.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>

struct meshdata_s {
  vertex_t *vertices;
  int pos;
  int size;
};

struct {
  GLuint vbo;
  int offset;
  int max_vertices;
} vbuffer;

void vbuffer_init(int max_vertices)
{
  glGenBuffers(1, &vbuffer.vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbuffer.vbo);
  glBufferData(GL_ARRAY_BUFFER, max_vertices * sizeof(vertex_t), 0, GL_STATIC_DRAW);
  
  // vertex.p
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (float*) 0);
  
  // vertex.n
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (float*) 0 + 3);
  
  // vertex.t
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (float*) 0 + 6);
  
  // vertex.bt 
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (float*) 0 + 9);
  
  // vertex.uv
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (float*) 0 + 12);
  
  vbuffer.offset = 0;
  vbuffer.max_vertices = max_vertices;
}

void vbuffer_bind()
{
  glBindBuffer(GL_ARRAY_BUFFER, vbuffer.vbo);
}

mesh_t vbuffer_add(meshdata_t md)
{
  int num_vertices = meshdata_get_size(md);
  
  if (vbuffer.offset + num_vertices > vbuffer.max_vertices) {
    fprintf(stderr, "error:%s:%s:%i: vbuffer out of memory\n", __FILE__, __FUNCTION__, __LINE__);
    exit(1);
  }
  
  mesh_t mesh = {
    .offset = vbuffer.offset,
    .count = num_vertices
  };
  
  vbuffer.offset += num_vertices;
  
  glBufferSubData(
    GL_ARRAY_BUFFER,
    mesh.offset * sizeof(vertex_t),
    num_vertices * sizeof(vertex_t),
    meshdata_get_vertices(md)
  );
  
  return mesh;
}

void vbuffer_draw(mesh_t mesh)
{
  glDrawArrays(GL_TRIANGLES, mesh.offset, mesh.count);
}

void vbuffer_deinit()
{
  glDeleteBuffers(1, &vbuffer.vbo);
}

meshdata_t meshdata_create()
{
  meshdata_t md = calloc(sizeof(*md), 1);
  md->pos = 0;
  md->size = 32;
  md->vertices = calloc(sizeof(*md->vertices), md->size);
  return md;
}

void meshdata_add_quad(meshdata_t md, matrix T_p)
{
  vector quad[] = {
    vec2(-1, -1),
    vec2(-1, +1),
    vec2(+1, -1),
    vec2(-1, +1),
    vec2(+1, +1),
    vec2(+1, -1)
  };
  
  matrix T_uv = mdotm(translate(vec2(1.0, 1.0)), scale(vec2(0.5, 0.5)));
  
  for (int i = 0; i < sizeof(quad) / sizeof(quad[0]); i++) {
    vector p = mdotv(T_p, v2pt(quad[i]));
    vector uv = mdotv(T_uv, v2pt(quad[i]));
    meshdata_add_vertex(md, vertex_create(p, uv));
  }
}

void meshdata_add_vertex(meshdata_t md, vertex_t v)
{
  if (md->pos >= md->size) {
    md->size *= 2;
    md->vertices = realloc(md->vertices, md->size);
  }
  
  md->vertices[md->pos++] = v;
}

const vertex_t *meshdata_get_vertices(meshdata_t md)
{
  return md->vertices;
}

int meshdata_get_size(meshdata_t md)
{
  return md->pos;
}

void meshdata_destroy(meshdata_t md)
{
  free(md->vertices);
  free(md);
}

