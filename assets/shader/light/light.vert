layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_tangent;
layout(location = 2) in vec3 v_bitangent;
layout(location = 3) in vec3 v_normal;
layout(location = 4) in vec2 v_uv;

out vec3 vs_pos;
out vec3 vs_normal;
out vec2 vs_uv;

void main()
{
  vs_uv = v_uv;
  vs_normal = v_normal;
  vs_pos = vec3(get_model() * vec4(v_pos, 1.0));
  gl_Position = get_mvp() * vec4(v_pos, 1.0);
}
