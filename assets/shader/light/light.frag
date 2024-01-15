out vec4 frag_color;

in vec3 vs_pos;
in vec3 vs_normal;
in vec2 vs_uv;

void main()
{
  vec3 light = vec3(0.0);
  
  for (int i = 0; i < MAX_POINTS; i++) {
    if (points[i].intensity <= 0.0) {
      continue;
    }
    
    float shadow = 1.0 - calc_point_shadow(i, vs_pos);
    
    light += calc_light(vs_pos, vs_normal, i) * shadow;
  }
  
  light += 0.01;
  
  frag_color = get_diffuse() * vec4(light, 1.0);
}
