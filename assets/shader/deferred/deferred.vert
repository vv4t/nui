layout(location = 0) in vec3 v_p;
layout(location = 4) in vec2 v_uv;

out vec2 vs_uv;
out vec3 vs_ray;

void main()
{
  vs_uv = v_uv;
  vs_ray = inverse(mat3(view)) * vec3(v_p.xy, 1.0);
  gl_Position = vec4(v_p, 1.0);
}
