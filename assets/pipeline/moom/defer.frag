out vec4 frag_color;

void main()
{
  if (get_frag_pos().z < 0.0) {
    frag_color = vec4(vec3(0.0), 1.0);
    return;
  }
  
  vec3 frag_normal = get_frag_normal();
  vec3 frag_pos = get_world_pos();
  
  vec3 light = vec3(0.0);
  vec3 fog = vec3(0.0);
  
  for (int i = 0; i < MAX_POINTS; i++) {
    if (points[i].intensity <= 0.0) {
      continue;
    }
    
    float shadow = 1.0 - calc_point_shadow(i, frag_pos);
    light += calc_light(frag_pos, frag_normal, get_specular(), i) * shadow;
  }
  
  frag_color = get_diffuse() * vec4(light, 1.0) + vec4(fog, 1.0);
}
