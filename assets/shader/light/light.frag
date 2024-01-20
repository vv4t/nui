out vec4 frag_color;

in vec3 vs_pos;
in vec3 vs_normal;
in vec2 vs_uv;
in mat3 vs_TBN;

uniform samplerCube u_skybox;

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
  
  light += 0.01;
  light += get_specular() * texture(u_skybox, R).rgb * get_alpha();
  
  frag_color = get_diffuse() * get_alpha() * vec4(light, 1.0);
}
