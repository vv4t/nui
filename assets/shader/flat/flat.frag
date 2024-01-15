out vec4 frag_color;

in vec2 vs_uv;

void main()
{
  frag_color = get_diffuse();
}
