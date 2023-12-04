#define CUBE_FACES 6
#define POINTS_MAX 2

out vec4 frag_color;

struct point_t {
  vec3  pos;
  float intensity;
  vec4  color;
};

struct point_shadow_t {
  mat4 light_matrices[CUBE_FACES];
};

layout (std140) uniform ub_light {
  point_t points[POINTS_MAX];
  point_shadow_t point_shadows[POINTS_MAX];
};

in vec3 vs_pos;
in vec3 vs_normal;
in vec2 vs_uv;

in vec4 vs_light_pos[CUBE_FACES * POINTS_MAX];

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
    
    float shadow = 0.0;
    
    for (int j = 0; j < CUBE_FACES; j++) {
      vec4 shadow_pos = vs_light_pos[i * CUBE_FACES + j];
      vec3 shadow_coord = shadow_pos.xyz / shadow_pos.w * 0.5 + vec3(0.5, 0.5, 0.5);
      
      if (shadow_coord.z < 0.0 || shadow_coord.z > 1.0) {
        continue;
      }
      
      for (int k = 0; k < 4; k++) {
        vec2 shadow_uv = shadow_coord.xy;
        
        vec2 poissonDisk[4] = vec2[](
          vec2( -0.94201624, -0.39906216 ),
          vec2( 0.94558609, -0.76890725 ),
          vec2( -0.094184101, -0.92938870 ),
          vec2( 0.34495938, 0.29387760 )
        );
        
        if (
          shadow_uv.x > 0.0 && shadow_uv.x < 1.0 &&
          shadow_uv.y > 0.0 && shadow_uv.y < 1.0
        ) {
          shadow_uv += poissonDisk[k] / 700.0;
          shadow_uv.x = (shadow_uv.x + float(j)) / float(CUBE_FACES);
          shadow_uv.y = (shadow_uv.y + float(i)) / float(POINTS_MAX);
          
          float closest_depth = texture(u_depth_map, shadow_uv).z;
          float current_depth = shadow_coord.z;
          
          float cos_theta = clamp(dot(normal, light_dir), 0.0, 1.0);
          float beta = 0.001 / shadow_pos.z;
          float bias = clamp(beta * 5.0 * tan(acos(cos_theta)), 0.0, beta);
          
          if (current_depth - bias > closest_depth) {
            shadow += 0.25;
          }
        }
      }
    }
    
    light += points[i].color.xyz * intensity * (1.0 - shadow);
  }
  
  light += vec3(0.1, 0.1, 0.1);
  
  vec4 diffuse = texture(u_color, vs_uv);
  
  frag_color = diffuse * vec4(light, 1.0);
}
