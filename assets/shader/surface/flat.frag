out vec4 frag_color;

void surface_shade(
  in vec3   frag_pos,
  in vec3   world_pos,
  vec2      uv,
  vec3      normal,
  sampler2D albedo)
{
  frag_color = texture(albedo, uv);
}
