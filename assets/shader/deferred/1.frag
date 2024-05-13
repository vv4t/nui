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
  
  vec3 R = reflect(normalize(world_pos - view_pos), normal);
  light += textureCube(u_cubemap, R).xyz * 0.5;
  
  frag_color = vec4(albedo * light, 1.0);
}
