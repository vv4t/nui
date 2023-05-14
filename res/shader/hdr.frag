out vec4 frag_color;

in vec2 vs_uv;

uniform sampler2D u_scene;

void main()
{
  const float gamma = 0.7;
  vec3 hdr_color = texture(u_scene, vs_uv).rgb;
  
  vec3 mapped = vec3(1.0) - exp(-hdr_color * 3.0);
  mapped = pow(mapped, vec3(1.0 / gamma));
  
  frag_color = vec4(mapped, 1.0);
}
