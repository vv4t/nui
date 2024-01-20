in vec2 vs_uv;
in vec3 vs_ray;

uniform sampler2D u_pos;
uniform sampler2D u_normal;
uniform sampler2D u_albedo;

#define get_frag_pos() texture(u_pos, vs_uv).rgb
#define get_world_pos() (normalize(vs_ray) * texture(u_pos, vs_uv).w + get_view_pos())
#define get_frag_normal() texture(u_normal, vs_uv).rgb
#define get_diffuse() texture(u_albedo, vs_uv)
#define get_specular() texture(u_albedo, vs_uv).w
