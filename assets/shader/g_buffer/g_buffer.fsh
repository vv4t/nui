layout (location = 0) out vec4 g_pos;
layout (location = 1) out vec4 g_normal;
layout (location = 2) out vec4 g_albedo;

in vec2 vs_uv;
in vec3 vs_pos;
in vec3 vs_world_pos;
in vec3 vs_normal;

uniform sampler2D texture_diffuse;

void main()
{
  float view_dist = length(vs_world_pos - view_pos);
  
  g_pos = vec4(vs_pos, 1.0);
  g_normal = vec4(vs_normal, view_dist);
  g_albedo = texture(texture_diffuse, vs_uv);
}
