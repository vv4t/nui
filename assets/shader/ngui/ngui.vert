layout(location = 0) in vec3 v_pos;
layout(location = 4) in vec2 v_uv;

out vec2 vs_uv;

void main()
{
  vs_uv = v_uv;
  gl_Position = vec4(v_pos * vec3(720.0 / 1280.0, 1.0, 1.0) * 2.0 - 1.0, 1.0);
}
