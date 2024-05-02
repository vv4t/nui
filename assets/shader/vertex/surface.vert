layout(location = 0) in vec3 v_p;
layout(location = 1) in vec3 v_n;
layout(location = 2) in vec3 v_t;
layout(location = 3) in vec3 v_bt;
layout(location = 4) in vec2 v_uv;

out vec2 vs_uv;
out vec3 vs_normal;
out vec3 vs_pos;

void main()
{
  vs_normal = normalize(mat3(transpose(inverse(model))) * v_n);
  vs_pos = vec3(model * vec4(v_p, 1.0));
  vs_uv = vec2(v_uv.x, 1.0 - v_uv.y);
  gl_Position = mvp * vec4(v_p, 1.0);
}
