layout (location = 0) out vec4 frag_color;

layout (std140) uniform ubo_matrices {
  mat4  mvp;
  mat4  model;
  vec3  view_pos;
  float pad[1];
};

uniform vec4 u_color;

void main() {
  frag_color = u_color;
}
