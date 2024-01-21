out vec4 frag_color;

uniform samplerCube u_skybox;
uniform samplerCube u_cubemap;

void main()
{
  if (get_frag_pos().z < 0.0) {
    frag_color = texture(u_skybox, vec3(-vs_ray.x, vs_ray.y, vs_ray.z));
    
    for (int i = 0; i < MAX_POINTS; i++) {
      frag_color.xyz += calc_fog(vs_ray * 1000.0 + get_view_pos(), i);
    }
    
    return;
  }
  
  vec3 frag_normal = get_frag_normal();
  vec3 frag_pos = get_world_pos();
  
  vec3 light = vec3(0.0);
  vec3 fog = vec3(0.0);
  
  vec3 I = normalize(frag_pos - get_view_pos());
  vec3 R = reflect(I, normalize(frag_normal));
  
  for (int i = 0; i < MAX_POINTS; i++) {
    if (points[i].intensity <= 0.0) {
      continue;
    }
    
    float shadow = 1.0 - calc_point_shadow(i, frag_pos);
    
    light += calc_light(frag_pos, frag_normal, get_specular(), i) * shadow;
    fog += calc_fog(frag_pos, i);
  }
  
  float ao = calc_ssao(u_pos, frag_pos, frag_normal);
  
  light += 0.1;
  light *= ao * ao;
  light += get_specular() * texture(u_cubemap, vec3(-R.x, R.y, R.z)).rgb;
  
  frag_color = get_diffuse() * vec4(light, 1.0) + vec4(fog, 1.0);
}
