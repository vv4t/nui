vec3 calc_ssr(
  sampler2D _albedo,
  sampler2D _position,
  vec3 world_pos,
  vec3 frag_pos,
  vec2 uv,
  vec3 normal
);

in vec3 vs_ray;

uniform samplerCube u_cubemap;

void deferred_shade(
  out vec4  frag_color,
  sampler2D _albedo,
  sampler2D _position,
  sampler2D _normal,
  vec2      uv)
{
  vec4 frag_pos = texture(_position, uv);
  
  if (frag_pos.w < 0.1) {
    frag_color = textureCube(u_cubemap, vs_ray);
    for (int i = 0; i < POINT_MAX; i++) {
      frag_color.xyz += calc_point_scatter(i, vs_ray * 1000.0, view_pos);
    }
    return;
  }
  
  vec3 world_pos = frag_pos.w * normalize(vs_ray) + view_pos;
  vec3 normal = texture(_normal, uv).xyz;
  vec3 albedo = texture(_albedo, uv).xyz;
  
  vec3 light = vec3(0.0);
  
  for (int i = 0; i < POINT_MAX; i++) {
    light += calc_point_light(i, world_pos, normal, view_pos);
    light += calc_point_scatter(i, world_pos, view_pos);
  }
  
  vec3 R = calc_ssr(_albedo, _position, world_pos, frag_pos.xyz, uv, normal) * 0.1;
  
  frag_color = vec4(albedo * light + R, 1.0);
  // frag_color = vec4(calc_ssr(_albedo, _position, world_pos, frag_pos.xyz, uv, normal), 1.0);
  // frag_color = frag_pos;
}

vec3 calc_ssr(
  sampler2D _albedo,
  sampler2D _position,
  vec3      world_pos,
  vec3      frag_pos,
  vec2      uv,
  vec3      normal
) {
  vec3 R = normalize(reflect(world_pos - view_pos, normal));
  
  float d_dist = 0.025 * length(world_pos - view_pos);
  float ray_dist = d_dist;
  
  for (int i = 0; i < 128; i++) {
    ray_dist += d_dist;
    
    vec4 ray_pos = project * view * vec4(world_pos + R * ray_dist, 1.0);
    vec2 ray_uv = (ray_pos.xyz / ray_pos.w).xy * 0.5 + 0.5;
    
    float frag_depth = texture(_position, ray_uv).z;
    
    if (frag_depth < 0.1) break;
    
    if (frag_depth < ray_pos.z) {
      return texture(_albedo, ray_uv).xyz;
    }
  }
  
  return textureCube(u_cubemap, R).xyz;
}
