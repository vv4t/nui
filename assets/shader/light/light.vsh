#define CUBE_FACES 6
#define POINTS_MAX 2
#define SHADOWS_MAX (POINTS_MAX * CUBE_FACES)

layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_tangent;
layout(location = 2) in vec3 v_bitangent;
layout(location = 3) in vec3 v_normal;
layout(location = 4) in vec2 v_uv;

layout (std140) uniform ubo_matrices {
  mat4  mvp;
  mat4  model;
};

struct point_t {
  vec3  pos;
  float intensity;
  vec4  color;
};

struct shadow_t {
  mat4 light_matrix;
};

layout (std140) uniform ub_light {
  point_t points[POINTS_MAX];
  shadow_t shadows[SHADOWS_MAX];
};

out vec3 vs_pos;
out vec3 vs_normal;
out vec2 vs_uv;

out vec4 vs_light_pos[SHADOWS_MAX];

void main()
{
  vs_uv = v_uv;
  vs_normal = v_normal;
  vs_pos = vec3(model * vec4(v_pos, 1.0));
  
  for (int i = 0; i < SHADOWS_MAX; i++) {
    vs_light_pos[i] = shadows[i].light_matrix * model * vec4(v_pos, 1.0);
  }
  
  gl_Position = mvp * vec4(v_pos, 1.0);
}
