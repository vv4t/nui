out vec4 frag_color;

uniform samplerCube u_skybox;

vec3 calc_ssr(vec3 frag_pos, vec3 frag_normal)
{
  vec3 I = normalize(frag_pos - get_view_pos());
  vec3 R = reflect(I, normalize(frag_normal));
  
  float ray_dist = 0.25;
  
  vec2 ray_uv = vs_uv;
  vec3 ray_pos = get_frag_pos();
  
  vec2 texel = vec2(textureSize(u_pos, 0));
  
  vec4 reflect_pos = get_mvp() * vec4(frag_pos + R * ray_dist, 1.0);
  vec2 reflect_uv = (reflect_pos.xyz / reflect_pos.w).xy * 0.5 + 0.5;
  
  vec2 diff_uv = reflect_uv - ray_uv;
  vec3 diff_pos = reflect_pos.xyz - ray_pos;
  
  for (int i = 0; i < 4; i++) {
    if (abs(diff_uv.x) < 1.0 / texel.x && abs(diff_uv.y) < 1.0 / texel.y) {
      ray_dist *= 2.0;
      
      reflect_pos = get_mvp() * vec4(frag_pos + R * ray_dist, 1.0);
      reflect_uv = (reflect_pos.xyz / reflect_pos.w).xy * 0.5 + 0.5;
      
      diff_uv = reflect_uv - ray_uv;
      diff_pos = reflect_pos.xyz - ray_pos;
    } else {
      break;
    }
  }
  
  for (int i = 0; i < 32; i++) {
    ray_uv += diff_uv;
    ray_pos += diff_pos;
    
    float frag_depth = texture(u_pos, ray_uv).z;
    
    if (
      ray_uv.x < 0.0 || ray_uv.y < 0.0 ||
      ray_uv.x > 1.0 || ray_uv.y > 1.0 ||
      ray_pos.z < 0.0 || frag_depth < 0.0
    ) {
      return texture(u_skybox, R).rgb;
    }
    
    if (frag_depth < ray_pos.z) {
      if (frag_depth < get_frag_pos().z) {
        return texture(u_skybox, R).rgb;
      }
      
      return texture(u_albedo, ray_uv).xyz;
    }
  }
  
  return texture(u_skybox, R).rgb;
}

void main()
{
  if (get_frag_pos().z < 0.0) {
    frag_color = texture(u_skybox, vs_ray);
    
    for (int i = 0; i < MAX_POINTS; i++) {
      frag_color.xyz += calc_fog(vs_ray * 1000.0 + get_view_pos(), i);
    }
    
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
    fog += calc_fog(frag_pos, i);
  }
  
  vec3 ssr = calc_ssr(frag_pos, frag_normal);
  
  float ao = calc_ssao(u_pos, frag_pos, frag_normal);
  
  light += 0.1;
  light *= ao * ao;
  light += ssr * get_specular() * 0.1;
  
  frag_color = get_diffuse() * vec4(light, 1.0) + vec4(fog, 1.0);
}
