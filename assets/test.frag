layout(location = 0) out vec4 frag_color;
layout(location = 1) out vec4 other_color;

void main()
{
  frag_color = vec4(1.0, 0.0, 0.0, 1.0);
  other_color = vec4(0.0, 0.0, 1.0, 1.0);
}
