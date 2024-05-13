uniform sampler2D screen;

void frame_shade(out vec4 frag_color, in vec2 frag_coord)
{
  const float gamma = 0.3;
  const float exposure = 4.1;
  
  vec3 hdr_color = texture(screen, frag_coord).rgb;
  
  vec3 mapped = vec3(1.0) - exp(-hdr_color * exposure);
  mapped = pow(mapped, vec3(1.0 / gamma));
  
  frag_color = vec4(mapped, 1.0);
}
