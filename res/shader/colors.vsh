layout(location = 0) in vec3 v_pos;

layout (std140) uniform ubo_matrices {
  mat4  mvp;
  mat4  model;
};

void main() {
  gl_Position = mvp * vec4(v_pos, 1.0);
}
