out vec4 frag_color;

in vec2 vs_uv;
in float vs_depth;

uniform sampler2D u_color;

void main()
{
  float depth = 0.1 + 2.0 / vs_depth;
  frag_color = vec4(depth, depth, depth, 1.0) * texture(u_color, vs_uv);
}
