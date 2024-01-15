#define CUBE_FACES 6
#define MAX_POINTS 8

struct point_t {
  vec3  pos;
  float intensity;
  vec4  color;
};

struct point_shadow_t {
  mat4 light_matrices[CUBE_FACES];
};

layout (std140) uniform ub_light {
  point_t points[MAX_POINTS];
  point_shadow_t point_shadows[MAX_POINTS];
};

uniform sampler2D u_depth_map;

vec3 calc_light(vec3 frag_pos, vec3 frag_normal, int id)
{
  vec3 delta_pos = points[id].pos - frag_pos;
  vec3 light_dir = normalize(delta_pos);
  
  vec3 view_dir = normalize(frag_pos - view_pos);
  vec3 reflect_dir = reflect(-light_dir, frag_normal);
  
  float specular = 0.4 * pow(max(dot(-view_dir, reflect_dir), 0.0), 32.0);
  float diffuse = max(dot(frag_normal, light_dir), 0.0);
  float delta_dist = length(delta_pos);
  
  float attenuation = points[id].intensity / (1.0 + 4.0 * delta_dist + 0.4 * delta_dist * delta_dist);
  float intensity = (diffuse + specular) * attenuation;
  
  vec3 light_color = points[id].color.xyz * intensity;
  
  return light_color;
}

float calc_point_shadow_face(int id, int face, vec3 light_dir, vec3 frag_pos)
{
  vec4 shadow_pos = point_shadows[id].light_matrices[face] * vec4(frag_pos, 1.0);
  vec3 shadow_coord = shadow_pos.xyz / shadow_pos.w * 0.5 + vec3(0.5, 0.5, 0.5);
  
  if (shadow_coord.z < 0.0 || shadow_coord.z > 1.0) {
    return 0.0;
  }
  
  vec2 shadow_uv = shadow_coord.xy;
  
  if (shadow_uv.x > 0.0 && shadow_uv.x < 1.0 && shadow_uv.y > 0.0 && shadow_uv.y < 1.0) {
    shadow_uv.x = clamp(shadow_uv.x, 0.001, 0.999);
    shadow_uv.y = clamp(shadow_uv.y, 0.001, 0.999);
    
    shadow_uv.x = (shadow_uv.x + float(face)) / float(CUBE_FACES);
    shadow_uv.y = (shadow_uv.y + float(id)) / float(MAX_POINTS);
    
    float closest_depth = texture(u_depth_map, shadow_uv).z;
    float current_depth = shadow_coord.z;
    
    float beta = 0.001 / shadow_pos.z;
    float bias = clamp(beta * 5.0, 0.0, beta);
    
    if (current_depth - bias > closest_depth) {
      return 1.0;
    }
  }
  
  return 0.0;
}

float calc_point_shadow(int id, vec3 frag_pos)
{
  vec3 delta_pos = points[id].pos - frag_pos;
  vec3 light_dir = normalize(delta_pos);
  
  // https://gist.github.com/hypernewbie/f80958de2cb262f8de2d00e52e1a0052
  
	vec3 v = abs(light_dir);
  int face_id = 0;
  
	if(v.z >= v.x && v.z >= v.y) {
		face_id = (light_dir.z < 0.0) ? 4 : 5;
	} else if(v.y >= v.x) {
		face_id = light_dir.y < 0.0 ? 2 : 3;
	} else {
		face_id = light_dir.x < 0.0 ? 0 : 1;
	}
  
  return calc_point_shadow_face(id, face_id, light_dir, frag_pos);
}
