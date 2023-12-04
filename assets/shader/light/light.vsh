#define CUBE_FACES 6
#define POINTS_MAX 2

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

struct point_shadow_t {
  mat4 light_matrices[CUBE_FACES];
};

layout (std140) uniform ub_light {
  point_t points[POINTS_MAX];
  point_shadow_t point_shadows[POINTS_MAX];
};

out vec3 vs_pos;
out vec3 vs_normal;
out vec2 vs_uv;

out vec4 vs_light_pos[POINTS_MAX * CUBE_FACES];

void main()
{
  vs_uv = v_uv;
  vs_normal = v_normal;
  vs_pos = vec3(model * vec4(v_pos, 1.0));
  
  for (int i = 0; i < POINTS_MAX; i++) {
    for (int j = 0; j < CUBE_FACES; j++) {
      vs_light_pos[i * CUBE_FACES + j] = point_shadows[i].light_matrices[j] * model * vec4(v_pos, 1.0);
    }
  }
  
  gl_Position = mvp * vec4(v_pos, 1.0);
}
