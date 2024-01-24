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

vec3 calc_ssr(vec3 frag_pos, vec2 frag_uv, vec3 frag_normal)
{
  vec3 I = normalize(vs_pos - get_view_pos());
  vec3 R = reflect(I, normalize(frag_normal));
  
  float ray_dist = 0.25;
  
  vec2 ray_uv = frag_uv;
  vec3 ray_pos = frag_pos;
  
  vec2 texel = vec2(textureSize(d_pos, 0));
  
  vec4 reflect_pos = get_mvp() * vec4(vs_pos + R * ray_dist, 1.0);
  vec2 reflect_uv = (reflect_pos.xyz / reflect_pos.w).xy * 0.5 + 0.5;
  
  vec2 diff_uv = reflect_uv - ray_uv;
  vec3 diff_pos = reflect_pos.xyz - ray_pos;
  
  for (int i = 0; i < 16; i++) {
    if (abs(diff_uv.x) < 1.0 / texel.x && abs(diff_uv.y) < 1.0 / texel.y) {
      ray_dist += 0.1;
      
      reflect_pos = get_mvp() * vec4(vs_pos + R * ray_dist, 1.0);
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
    
    float frag_depth = texture(d_pos, ray_uv).z;
    
    if (
      ray_uv.x < 0.0 || ray_uv.y < 0.0 ||
      ray_uv.x > 1.0 || ray_uv.y > 1.0 ||
      ray_pos.z < 0.0 || frag_depth < 0.0
    ) {
      return texture(u_skybox, R).rgb;
    }
    
    if (frag_depth < ray_pos.z) {
      if (frag_depth < frag_pos.z) {
        return texture(u_skybox, R).rgb;
      }
      
      vec2 poisson_disk[4] = vec2[](
        vec2( -0.94201624, -0.39906216 ),
        vec2( 0.94558609, -0.76890725 ),
        vec2( -0.094184101, -0.92938870 ),
        vec2( 0.34495938, 0.29387760 )
      );
      
      vec3 color = vec3(0.0);
      
      for (int k = 0; k < 4; k++) {
        color += texture(d_albedo, ray_uv + poisson_disk[k] / 700.0).xyz * 0.25;
      }
      
      return color;
    }
  }
  
  return texture(u_skybox, R).rgb;
}

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
  light += get_specular() * texture(u_skybox, R).rgb * get_alpha();
  light += calc_ssr(vs_frag_pos.xyz, screen_uv, normal) * get_specular() * 2.0;
  
  frag_color = get_diffuse() * get_alpha() * vec4(light, 1.0);
}
