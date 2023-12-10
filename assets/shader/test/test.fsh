out vec4 frag_color;

in vec2 vs_uv;

uniform sampler2D u_pos;
uniform sampler2D u_normal;
uniform sampler2D u_albedo;

vec3 get_frag_pos()
{
  return texture(u_pos, vs_uv).rgb;
}

vec3 get_frag_normal()
{
  return texture(u_normal, vs_uv).rgb;
}

vec4 get_diffuse()
{
  return texture(u_albedo, vs_uv);
}

void main()
{
  frag_color = vec4(get_diffuse().rgb * get_frag_pos().z, 1.0);
}
