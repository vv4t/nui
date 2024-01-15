layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_tangent;
layout(location = 2) in vec3 v_bitangent;
layout(location = 3) in vec3 v_normal;
layout(location = 4) in vec2 v_uv;

out vec3 vs_pos;
out vec3 vs_normal;
out vec3 vs_world_pos;
out vec2 vs_uv;

out mat3 vs_TBN;

void main()
{
  vec3 tangent = mat3(mat_model) * v_tangent;
  vec3 bitangent = mat3(mat_model) * v_bitangent;
  vec3 normal = mat3(mat_model) * v_normal;
  
  vs_TBN = mat3(tangent, bitangent, normal);
  
  vs_uv = v_uv;
  vs_normal = v_normal;
  vs_world_pos = v_pos;
  gl_Position = mat_mvp * vec4(v_pos, 1.0);
  vs_pos = gl_Position.xyz;
}
