out vec4 frag_color;

in vec2 vs_uv;
uniform sampler2D u_scene;

void set_frag(vec4 v)
{
  frag_color = v;
}

#define get_color(uv) texture(u_scene, uv)
#define get_uv() vs_uv
