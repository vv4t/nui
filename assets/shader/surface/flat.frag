out vec4 frag_color;

in vec2 vs_uv;

uniform sampler2D albedo;

void main()
{
  frag_color = texture(albedo, vs_uv);
}
