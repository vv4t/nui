layout(location = 0) in vec3 v_pos;

void main() {
  gl_Position = get_mvp() * vec4(v_pos, 1.0);
}
