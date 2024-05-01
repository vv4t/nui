layout(location = 0) in vec3 v_p;
layout(location = 1) in vec3 v_n;
layout(location = 2) in vec3 v_t;
layout(location = 3) in vec3 v_bt;
layout(location = 4) in vec2 v_uv;

out vec2 vs_uv;

void main()
{
  vs_uv = vec2(v_uv.x, 1.0 - v_uv.y);
  gl_Position = mvp * vec4(v_p, 1.0);
}
