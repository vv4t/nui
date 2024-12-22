in vec2 vs_uv;

out vec4 frag_color;

uniform sampler2D u_texture;

void main()
{
  const float gamma = 1.5;
  const float exposure = 4.1;

  vec3 hdr_color = texture(u_texture, vs_uv).rgb;

  vec3 mapped = vec3(1.0) - exp(-hdr_color * exposure);
  mapped = pow(mapped, vec3(1.0 / gamma));

  frag_color = vec4(mapped, 1.0);
}
