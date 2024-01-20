layout (location = 0) out vec4 g_pos;
layout (location = 1) out vec4 g_normal;
layout (location = 2) out vec4 g_albedo;

in vec2 vs_uv;
in vec3 vs_pos;
in vec3 vs_world_pos;
in vec3 vs_normal;

in mat3 vs_TBN;

void main()
{
  float view_dist = length(vs_world_pos - view_pos);
  
  vec3 normal = texture(u_normal, vs_uv).xyz;
  normal = normal * 2.0 - 1.0;
  normal = normalize(vs_TBN * normal);
  
  g_pos = vec4(vs_pos, view_dist);
  g_normal = vec4(normal, 1.0);
  g_albedo = vec4(get_diffuse().xyz, get_specular());
}
