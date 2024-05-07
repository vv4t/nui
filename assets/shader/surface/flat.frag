out vec4 frag_color;

void surface_shade(
  in vec3 frag_pos,
  in vec2 uv,
  in mat3 TBN,
  sampler2D albedo
) {
  frag_color = texture(albedo, uv);
}
