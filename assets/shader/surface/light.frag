out vec4 frag_color;

in vec2 vs_uv;
in vec3 vs_pos;
in vec3 vs_normal;

uniform sampler2D u_albedo;

void surface_shade(
  in vec2 frag_pos,
  in vec2 uv,
  in mat3 TBN,
  sampler2D albedo,
  sampler2D normal
) {
  
}

void main()
{
  vec3 d = vec3(0.0, 1.4, 0.0) - vs_pos;
  float f = clamp(dot(vs_normal, normalize(d)), 0.0, 1.0);
  float l = 10.0 / dot(d, d);
  
  float a = l * f + 0.2;
  
  vec3 albedo = texture(u_albedo, vs_uv).xyz;
  frag_color = vec4(a * albedo, 1.0);
}
