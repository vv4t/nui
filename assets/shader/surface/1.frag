#define AMBIENCE 0.5

out vec4 frag_color;

uniform samplerCube u_cubemap;

void surface_shade(
  in vec3   frag_pos,
  in vec3   world_pos,
  vec2      uv,
  vec3      normal,
  sampler2D albedo)
{
  vec3 light = vec3(0.0);
  
  for (int i = 0; i < POINT_MAX; i++) {
    light += calc_point_light(i, frag_pos, normal, view_pos);
    light += calc_point_scatter(i, frag_pos, view_pos);
  }
  
  vec3 R = reflect(normalize(frag_pos - view_pos), normal);
  light += textureCube(u_cubemap, R).xyz * 0.5;
  
  frag_color = vec4(light.xyz * texture(albedo, uv).xyz, 1.0);
}
