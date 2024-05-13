#define AMBIENCE 0.5

layout (location = 0) out vec4 g_albedo;
layout (location = 1) out vec4 g_position;
layout (location = 2) out vec4 g_normal;

void surface_shade(
  in vec3   frag_pos,
  in vec3   world_pos,
  vec2      uv,
  vec3      normal,
  sampler2D albedo)
{
  g_albedo = texture(albedo, uv);
  g_position = vec4(frag_pos, length(world_pos - view_pos));
  g_normal = vec4(normal, 1.0);
}
