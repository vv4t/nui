#define AMBIENCE 0.5

layout (location = 0) out vec4 g_albedo;
layout (location = 1) out vec4 g_position;
layout (location = 2) out vec4 g_normal;

void surface_shade(
  in vec3 frag_pos,
  vec2 uv,
  vec3 normal,
  sampler2D albedo
) {
  g_albedo = texture(albedo, uv);
  g_position = vec4(frag_pos, 1.0);
  g_normal = vec4(normal, 1.0);
}
