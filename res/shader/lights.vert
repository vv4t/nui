layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_tangent;
layout(location = 2) in vec3 v_bitangent;
layout(location = 3) in vec3 v_normal;
layout(location = 4) in vec2 v_uv;

layout (std140) uniform ubo_matrices {
  mat4  mvp;
  mat4  model;
  vec3  view_pos;
  float pad[1];
};

struct light_t {
  mat4  light_matrices[6];
  vec3  pos;
  float intensity;
  vec3  color;
  float pad[1];
};

layout (std140) uniform ubo_lights {
  light_t lights[MAX_LIGHTS];
};

out vec2 vs_uv;
out vec3 vs_pos;
out vec3 vs_normal;
out mat3 vs_TBN;

out vec4 vs_light_pos[MAX_LIGHTS * 6];

void main() {
  vec3 tangent = vec3(model * vec4(v_tangent, 1.0));
  vec3 bitangent = vec3(model * vec4(v_bitangent, 1.0));
  vec3 normal = vec3(model * vec4(v_normal, 1.0));
  
  vs_TBN = mat3(tangent, bitangent, normal);
  vs_pos = vec3(model * vec4(v_pos, 1.0));
  vs_uv = v_uv;
  vs_normal = normal;
  
  for (int i = 0; i < MAX_LIGHTS; i++) {
    for (int j = 0; j < 6; j++) {
      vs_light_pos[i * 6 + j] = lights[i].light_matrices[j] * vec4(v_pos, 1.0);
    }
  }
  
  gl_Position = mvp * vec4(v_pos, 1.0);
}
