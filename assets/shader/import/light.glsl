#define CUBE_FACES 6
#define MAX_POINTS 8

struct point_t {
  vec3  pos;
  float intensity;
  vec4  color;
};

layout (std140) uniform ub_light {
  point_t points[MAX_POINTS];
};

uniform sampler2D u_depth_map;

vec3 calc_light(vec3 frag_pos, vec3 frag_normal, float frag_specular, int id)
{
  vec3 delta_pos = points[id].pos - frag_pos;
  vec3 light_dir = normalize(delta_pos);
  
  vec3 view_dir = normalize(frag_pos - view_pos);
  vec3 reflect_dir = reflect(-light_dir, frag_normal);
  
  float specular = frag_specular * pow(max(dot(-view_dir, reflect_dir), 0.0), 32.0);
  float diffuse = max(dot(frag_normal, light_dir), 0.0);
  float delta_dist = length(delta_pos);
  
  float attenuation = points[id].intensity / (1.0 + 4.0 * delta_dist + 0.4 * delta_dist * delta_dist);
  float intensity = (diffuse + specular) * attenuation;
  
  vec3 light_color = points[id].color.xyz * intensity;
  
  return light_color;
}

struct cube_sample_t { float face; vec2 uv; vec3 axis; };

// https://gist.github.com/hypernewbie/f80958de2cb262f8de2d00e52e1a0052
cube_sample_t cube_sample_cube(vec3 v, vec2 poisson)
{
  cube_sample_t cube_sample;
  
  vec3 vAbs = abs(v);
  float ma;
  
  if (vAbs.z >= vAbs.x && vAbs.z >= vAbs.y) {
    cube_sample.face = v.z < 0.0 ? 5.0 : 4.0;
    ma = 0.5 / vAbs.z;
    cube_sample.uv = vec2(v.z > 0.0 ? -v.x : v.x, -v.y);
    cube_sample.axis = vec3(0.0, 0.0, v.z > 0.0 ? 1.0 : -1.0);
  } else if (vAbs.y >= vAbs.x) {
    cube_sample.face = v.y < 0.0 ? 3.0 : 2.0;
    ma = 0.5 / vAbs.y;
    cube_sample.uv = vec2(-v.x, v.y < 0.0 ? -v.z : v.z);
    cube_sample.axis = vec3(0.0, v.y > 0.0 ? 1.0 : -1.0, 0.0);
  } else {
    cube_sample.face = v.x < 0.0 ? 1.0 : 0.0;
    ma = 0.5 / vAbs.x;
    cube_sample.uv = vec2(v.x > 0.0 ? v.z : -v.z, -v.y);
    cube_sample.axis = vec3(v.x > 0.0 ? 1.0 : -1.0, 0.0, 0.0);
  }

  cube_sample.uv = cube_sample.uv * ma + 0.5 + poisson;
  cube_sample.uv.x = clamp(cube_sample.uv.x, 0.001, 0.999);
  cube_sample.uv.y = clamp(cube_sample.uv.y, 0.001, 0.999);
  
  return cube_sample;
}

float depth_cube_cube_sample(int id, cube_sample_t cube_sample)
{
  vec2 uv = cube_sample.uv;  
  
  uv.x = (uv.x + cube_sample.face) / float(CUBE_FACES);
  uv.y = (uv.y + float(id)) / float(MAX_POINTS);
  
  return texture(u_depth_map, uv).r;
}

float calc_point_shadow(int id, vec3 frag_pos)
{
  vec3 delta_pos = frag_pos - points[id].pos;
  vec3 light_dir = normalize(delta_pos);
  
  float near = 0.1;
  float far = 100.0;
  float z_scale = (-near - far) / (near - far);
  float z_offset = (2.0 * far * near) / (near - far);
  
  float shadow = 0.0;
  
  vec2 poisson_disk[4] = vec2[](
    vec2( -0.94201624, -0.39906216 ),
    vec2( 0.94558609, -0.76890725 ),
    vec2( -0.094184101, -0.92938870 ),
    vec2( 0.34495938, 0.29387760 )
  );
  
  for (int k = 0; k < 4; k++) {
    cube_sample_t cube_sample = cube_sample_cube(light_dir, poisson_disk[k] / 700.0);
    
    float z_view = dot(delta_pos, cube_sample.axis);
    float z_depth = (z_view * z_scale + z_offset) / z_view;
    
    float closest_depth = depth_cube_cube_sample(id, cube_sample);
    float current_depth = z_depth * 0.5 + 0.5;
    
    float bias = 0.001 / z_view;
    
    if (current_depth - bias > closest_depth) {
      shadow += 0.25;
    }
  }
  
  return shadow;
}

vec3 calc_fog(vec3 frag_pos, int id)
{
  vec3 delta_pos = points[id].pos - frag_pos;
  vec3 light_dir = normalize(delta_pos);
  
  vec3 view_dir = normalize(frag_pos - view_pos);
  
  vec3 dir = normalize(light_dir - view_dir * dot(light_dir, view_dir));
  float h = dot(points[id].pos, dir) - dot(view_pos, dir);
  float c = dot(points[id].pos, view_dir);
  float a = dot(view_pos, view_dir) - c;
  float b = dot(frag_pos, view_dir) - c;
  float fog = atan(b / h) / h - atan(a / h) / h;
  
  return points[id].color.xyz * vec3(0.01 * fog * fog + 0.05 * fog);
}
