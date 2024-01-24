out vec4 frag_color;

in vec3 vs_pos;
in vec3 vs_normal;
in vec2 vs_uv;
in mat3 vs_TBN;

in vec4 vs_frag_pos;

uniform samplerCube u_skybox;

uniform sampler2D d_pos;
uniform sampler2D d_normal;
uniform sampler2D d_albedo;

void main()
{
  vec3 normal = get_normal(vs_uv, vs_TBN);
  
  vec3 light = vec3(0.0);
  
  vec3 I = normalize(vs_pos - get_view_pos());
  vec3 R = reflect(I, normalize(normal));
  
  for (int i = 0; i < MAX_POINTS; i++) {
    if (points[i].intensity <= 0.0) {
      continue;
    }
    
    float shadow = 1.0 - calc_point_shadow(i, vs_pos);
    
    light += calc_light(vs_pos, normal, get_specular(), i) * shadow;
  }
  
  vec3 screen_coord = vs_frag_pos.xyz / vs_frag_pos.w;
  vec2 screen_uv = screen_coord.xy * 0.5 + 0.5;
  
  light += 0.1;
  light += calc_ssr(d_pos, d_albedo, u_skybox, vs_pos, vs_frag_pos.xyz, screen_uv, normal) * get_specular() * 4.0;
  
  frag_color = get_diffuse() * get_alpha() * vec4(light, 1.0);
}
