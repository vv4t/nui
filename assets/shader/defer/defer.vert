layout(location = 0) in vec3 v_pos;
layout(location = 4) in vec2 v_uv;

out vec2 vs_uv;
out vec3 vs_ray;

void main()
{
  vs_uv = v_uv;
  vs_ray = vec3(inverse(mat_look) * vec4(v_pos, 1.0));
  gl_Position = vec4(v_pos, 1.0);
}
