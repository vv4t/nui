out vec4 frag_color;

void main()
{
  if (get_frag_pos().z < 0.0) {
    frag_color = vec4(vec3(0.0), 1.0);
    
    for (int i = 0; i < MAX_POINTS; i++) {
      frag_color.xyz += calc_fog(vs_ray * 1000.0 + get_view_pos(), i);
    }
    
    return;
  }
  
  vec3 frag_normal = get_frag_normal();
  vec3 frag_pos = get_world_pos();
  
  vec3 fog = vec3(0.0);
  
  for (int i = 0; i < MAX_POINTS; i++) {
    if (points[i].intensity <= 0.0) {
      continue;
    }
    
    fog += calc_fog(frag_pos, i);
  }
  
  float occlude = calc_ssao(u_pos, frag_pos, frag_normal);
  occlude *= occlude; 

  frag_color = vec4(get_diffuse().rgb * occlude, 1.0) + vec4(fog, 1.0);
}
