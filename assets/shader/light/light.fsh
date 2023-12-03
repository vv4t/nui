#define CUBE_FACES 6
#define POINTS_MAX 3
#define SHADOWS_MAX (POINTS_MAX * CUBE_FACES)

out vec4 frag_color;

struct point_t {
  vec3  pos;
  float intensity;
  vec4  color;
};

struct shadow_t {
  mat4 light_matrix;
};

layout (std140) uniform ub_light {
  point_t points[POINTS_MAX];
  shadow_t shadows[SHADOWS_MAX];
};

in vec3 vs_pos;
in vec3 vs_normal;
in vec2 vs_uv;

in vec4 vs_light_pos[SHADOWS_MAX];

uniform sampler2D u_color;
uniform sampler2D u_depth_map;
uniform vec3 u_view_pos;

void main()
{
  vec3 light = vec3(0.0);
  
  vec3 normal = vs_normal;
  
  for (int i = 0; i < 2; i++) {
    if (points[i].intensity <= 0.0) {
      continue;
    }
    
    vec3 delta_pos = points[i].pos - vs_pos;
    vec3 light_dir = normalize(delta_pos);
    
    vec3 view_dir = normalize(u_view_pos - vs_pos);
    vec3 reflect_dir = reflect(-light_dir, normal);
    
    float specular = 0.4 * pow(max(dot(view_dir, reflect_dir), 0.0), 32.0);
    float diffuse = max(dot(normal, light_dir), 0.0);
    float delta_dist = length(delta_pos);
    
    float attenuation = points[i].intensity / (1.0 + 4.0 * delta_dist + 0.4 * delta_dist * delta_dist);
    float intensity = (diffuse + specular) * attenuation;
    
    float shadow = 1.0;
    
    for (int j = 0; j < CUBE_FACES; j++) {
      int shadow_idx = i * CUBE_FACES + j;
      vec4 shadow_pos = vs_light_pos[shadow_idx];
      vec3 shadow_coord = shadow_pos.xyz / shadow_pos.w * 0.5 + vec3(0.5, 0.5, 0.5);
      
      if (
        shadow_coord.x > 0.0 && shadow_coord.x < 1.0 &&
        shadow_coord.y > 0.0 && shadow_coord.y < 1.0 &&
        shadow_coord.z > 0.0 && shadow_coord.z < 1.0
      ) {
        vec2 shadow_uv = shadow_coord.xy;
        shadow_uv.x = (shadow_uv.x + float(shadow_idx)) / float(SHADOWS_MAX);
        
        float closest_depth = texture(u_depth_map, shadow_uv).z;
        float current_depth = shadow_coord.z;
        
        float cos_theta = dot(normal, -light_dir);
        float max_bias = 0.00013;
        float bias = clamp(max_bias * (1.0 - cos_theta), 0.0, max_bias);
        
        if (current_depth - bias < closest_depth) {
          shadow = 0.0;
        }
      }
    }
    
    light += points[i].color.xyz * intensity * (1.0 - shadow);
  }
  
  light += vec3(0.1, 0.1, 0.1);
  
  vec4 diffuse = texture(u_color, vs_uv);
  
  frag_color = diffuse * vec4(light, 1.0);
}
