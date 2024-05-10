#define POINT_MAX 8

struct ub_point_t {
  vec4 position;
  vec4 color;
};

layout (std140) uniform ub_light {
  ub_point_t point[POINT_MAX];
};

vec3 calc_point_light(int id, vec3 frag_pos, vec3 normal, vec3 view_pos)
{
  vec3 delta_pos = point[id].position.xyz - frag_pos;
  vec3 light_dir = normalize(delta_pos);
  float delta_dist = length(delta_pos);
  
  vec3 view_dir = normalize(frag_pos - view_pos);
  vec3 reflect_dir = reflect(-light_dir, normal);
  
  float specular = pow(max(dot(-view_dir, reflect_dir), 0.0), 32.0);
  float diffuse = clamp(dot(normal, normalize(delta_pos)), 0.0, 1.0);
  float intensity = point[id].color.w / (delta_dist * delta_dist + delta_dist);
  
  return point[id].color.xyz * intensity * (diffuse + specular * diffuse);
}

vec3 calc_point_scatter(int id, vec3 frag_pos, vec3 view_pos)
{
  vec3 delta_pos = point[id].position.xyz - frag_pos;
  vec3 light_dir = normalize(delta_pos);
  
  vec3 view_dir = normalize(frag_pos - view_pos);
  
  vec3 dir = normalize(light_dir - view_dir * dot(light_dir, view_dir));
  float h = dot(point[id].position.xyz, dir) - dot(view_pos, dir);
  float c = dot(point[id].position.xyz, view_dir);
  float a = dot(view_pos, view_dir) - c;
  float b = dot(frag_pos, view_dir) - c;
  float fog = atan(b / h) / h - atan(a / h) / h;
  
  return point[id].color.xyz * vec3(0.001 * fog * fog + 0.05 * fog);
}
