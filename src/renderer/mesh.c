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

static void solve_tangent(vertex_t *v1, vertex_t *v2, vertex_t *v3);

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
    vec2(-1, +1),
    vec2(-1, -1),
    vec2(+1, -1),
    
    vec2(-1, +1),
    vec2(+1, -1),
    vec2(+1, +1)
  };
  
  vector up = vec3(0, 0, -1);
  
  matrix T_n = mat3_from_mat4(T_p);
  matrix T_uv = mdotm(translate(vec2(1.0, 1.0)), scale(vec2(0.5, 0.5)));
  
  vector n = mdotv(T_n, v2pt(up));
  
  meshdata_add_face(
    md,
    vertex_create(mdotv(T_p, v2pt(quad[0])), n, mdotv(T_uv, v2pt(quad[0]))),
    vertex_create(mdotv(T_p, v2pt(quad[1])), n, mdotv(T_uv, v2pt(quad[1]))),
    vertex_create(mdotv(T_p, v2pt(quad[2])), n, mdotv(T_uv, v2pt(quad[2])))
  );
  
  meshdata_add_face(
    md,
    vertex_create(mdotv(T_p, v2pt(quad[3])), n, mdotv(T_uv, v2pt(quad[3]))),
    vertex_create(mdotv(T_p, v2pt(quad[4])), n, mdotv(T_uv, v2pt(quad[4]))),
    vertex_create(mdotv(T_p, v2pt(quad[5])), n, mdotv(T_uv, v2pt(quad[5])))
  );
}

void meshdata_add_face(meshdata_t md, vertex_t v1, vertex_t v2, vertex_t v3)
{
  solve_tangent(&v1, &v2, &v3);
  meshdata_add_vertex(md, v1);
  meshdata_add_vertex(md, v2);
  meshdata_add_vertex(md, v3);
}

void meshdata_add_vertex(meshdata_t md, vertex_t v)
{
  if (md->pos >= md->size) {
    md->size *= 2;
    md->vertices = realloc(md->vertices, md->size * sizeof(*md->vertices));
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

static void solve_tangent(vertex_t *v1, vertex_t *v2, vertex_t *v3)
{
  vector p1 = vec3(v1->p[0], v1->p[1], v1->p[2]);
  vector p2 = vec3(v2->p[0], v2->p[1], v2->p[2]);
  vector p3 = vec3(v3->p[0], v3->p[1], v3->p[2]);
  
  vector uv1 = vec2(v1->uv[0], v1->uv[1]);
  vector uv2 = vec2(v2->uv[0], v2->uv[1]);
  vector uv3 = vec2(v3->uv[0], v3->uv[1]);
  
  vector d_p1 = vsubv(p2, p1);
  vector d_p2 = vsubv(p3, p1);
  
  vector d_uv1 = vsubv(uv2, uv1);
  vector d_uv2 = vsubv(uv3, uv1);
  
  float f = 1.0f / (d_uv1.x * d_uv2.y - d_uv2.x * d_uv1.y);
  
  vector t = fdotv(f, vsubv(fdotv(d_uv2.y, d_p1), fdotv(d_uv1.y, d_p2)));
  vector bt = fdotv(f, vsubv(fdotv(d_uv1.x, d_p2), fdotv(d_uv2.x, d_p1)));
  
  v1->t[0] = t.x; v1->t[1] = t.y; v1->t[2] = t.z;
  v2->t[0] = t.x; v2->t[1] = t.y; v2->t[2] = t.z;
  v3->t[0] = t.x; v3->t[1] = t.y; v3->t[2] = t.z;
  
  v1->bt[0] = bt.x; v1->bt[1] = bt.y; v1->bt[2] = bt.z;
  v2->bt[0] = bt.x; v2->bt[1] = bt.y; v2->bt[2] = bt.z;
  v3->bt[0] = bt.x; v3->bt[1] = bt.y; v3->bt[2] = bt.z;
}