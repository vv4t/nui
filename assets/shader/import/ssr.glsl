
vec3 calc_ssr(
  sampler2D u_pos,
  sampler2D u_albedo,
  samplerCube env,
  vec3 frag_world_pos,
  vec3 frag_view_pos,
  vec2 frag_uv,
  vec3 frag_normal
) {
  vec3 I = normalize(frag_world_pos - get_view_pos());
  vec3 R = reflect(I, normalize(frag_normal));
  
  float ray_dist = 0.1;
  
  vec2 ray_uv = frag_uv;
  vec3 ray_pos = frag_view_pos;
  
  vec2 texel = vec2(textureSize(u_pos, 0));
  
  vec4 reflect_pos = get_mvp() * vec4(frag_world_pos + R * ray_dist, 1.0);
  vec2 reflect_uv = (reflect_pos.xyz / reflect_pos.w).xy * 0.5 + 0.5;
  
  vec2 diff_uv = reflect_uv - ray_uv;
  vec3 diff_pos = reflect_pos.xyz - ray_pos;
  
  for (int i = 0; i < 16; i++) {
    if (abs(diff_uv.x) < 1.0 / texel.x && abs(diff_uv.y) < 1.0 / texel.y) {
      ray_dist += 0.1;
      
      reflect_pos = get_mvp() * vec4(frag_world_pos + R * ray_dist, 1.0);
      reflect_uv = (reflect_pos.xyz / reflect_pos.w).xy * 0.5 + 0.5;
      
      diff_uv = reflect_uv - ray_uv;
      diff_pos = reflect_pos.xyz - ray_pos;
    } else {
      break;
    }
  }
  
  for (int i = 0; i < 256; i++) {
    ray_uv += diff_uv;
    ray_pos += diff_pos;
    
    float frag_depth = texture(u_pos, ray_uv).z;
    
    if (
      ray_uv.x < 0.0 || ray_uv.y < 0.0 ||
      ray_uv.x > 1.0 || ray_uv.y > 1.0 ||
      ray_pos.z < 0.0 || frag_depth < 0.0
    ) {
      return texture(env, R).rgb;
    }
    
    if (frag_depth < ray_pos.z) {
      if (frag_depth < frag_view_pos.z - 0.05) {
        return texture(env, R).rgb;
      }
      
      return texture(u_albedo, ray_uv).xyz;
      
      vec2 poisson_disk[4] = vec2[](
        vec2( -0.94201624, -0.39906216 ),
        vec2( 0.94558609, -0.76890725 ),
        vec2( -0.094184101, -0.92938870 ),
        vec2( 0.34495938, 0.29387760 )
      );
      
      vec3 color = vec3(0.0);
      
      for (int k = 0; k < 4; k++) {
        color += texture(u_albedo, ray_uv + poisson_disk[k] / 700.0).xyz * 0.25;
      }
      
      return color;
    }
  }
  
  return texture(env, R).rgb;
}
