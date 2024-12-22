#pragma use "camera.glsl"

layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec3 v_tangent;
layout(location = 3) in vec3 v_bitangent;
layout(location = 4) in vec2 v_uv;

out vec2 vs_uv;
out vec3 vs_pos;
out mat3 vs_TBN;
out vec3 vs_screen_normal;

void main()
{
  vec3 T = normalize(vec3(model * vec4(v_tangent, 0.0)));
  vec3 B = normalize(vec3(model * vec4(v_bitangent, 0.0)));
  vec3 N = normalize(vec3(model * vec4(v_normal, 0.0)));

  vs_TBN = mat3(T, B, N);
  vs_pos = (model * vec4(v_pos, 1.0)).xyz;
  vs_uv = (transpose(vs_TBN) * vs_pos).xy * 0.75;
  vs_screen_normal = normalize(vec3(MVP * vec4(N, 0.0)));

  gl_Position = MVP * vec4(v_pos, 1.0);
}
