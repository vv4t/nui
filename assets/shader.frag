in vec2 vs_uv;

out vec4 frag_color;

uniform sampler2D u_texture;

void main()
{
  frag_color = texture(u_texture, vs_uv);
}
