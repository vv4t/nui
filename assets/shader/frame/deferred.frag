uniform sampler2D albedo;
uniform sampler2D position;
uniform sampler2D normal;

void frame_shade(out vec4 frag_color, in vec2 frag_coord)
{
  frag_color = texture(position, frag_coord);
}
