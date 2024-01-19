layout(location = 0) in vec3 v_pos;
layout(location = 4) in vec2 v_uv;

out vec3 vs_uv;

void main()
{
  vs_uv = v_pos;
  gl_Position = get_look() * vec4(v_pos, 1.0);
}
