uniform sampler2D u_albedo;
uniform sampler2D u_position;
uniform sampler2D u_normal;

void frame_shade(out vec4 frag_color, in vec2 frag_coord)
{
  frag_color = texture(u_albedo, frag_coord);
}
