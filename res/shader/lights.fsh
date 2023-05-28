layout (location = 0) out vec4 frag_color;

in vec3 vs_pos;
in vec3 vs_normal;
in vec2 vs_uv;
in mat3 vs_TBN;

in vec4 vs_light_pos[MAX_LIGHTS * 6];

uniform sampler2D u_color;
uniform sampler2D u_normal;
uniform sampler2D u_depth_map;
uniform vec3      u_view_pos;

struct light_t {
  mat4  light_matrices[6];
  vec3  pos;
  float intensity;
  vec4  color;
};

layout (std140) uniform ubo_lights {
  light_t lights[MAX_LIGHTS];
};

layout (std140) uniform ubo_matrices {
  mat4  mvp;
  mat4  model;
};

void main() {
  vec3 light = vec3(0.0);
  
  vec3 normal = texture(u_normal, vs_uv).rgb;
  normal = normal * 2.0 - 1.0;
  normal = normalize(vs_TBN * normal);
  
  float v_map = 0.0;
  
  for (int i = 0; i < MAX_LIGHTS; i++) {
    if (lights[i].intensity <= 0.0)
      continue;
    
    vec3 delta_pos = lights[i].pos - vs_pos;
    vec3 light_dir = normalize(delta_pos);
  
    float shadow = 1.0;
    float u_map = 0.0;
    
    for (int j = 0; j < 6; j++) {
      vec3 proj_coords = vs_light_pos[i * 6 + j].xyz / vs_light_pos[i * 6 + j].w;
      
      if (
        proj_coords.x < 1.0 / 6.0 && proj_coords.x > 0.0 &&
        proj_coords.y < 1.0 / float(MAX_LIGHTS) && proj_coords.y > 0.0 && 
        proj_coords.z < 1.0 && proj_coords.z > 0.0
      ) {
        proj_coords.x = proj_coords.x + u_map;
        proj_coords.y = proj_coords.y + v_map;
        
        float closest_depth = texture(u_depth_map, proj_coords.xy).r;
        float current_depth = proj_coords.z;
        
        float cos_theta = dot(vs_normal, -light_dir);
        float bias = clamp(0.0005 * (1.0 - cos_theta), 0.0, 0.0001);
        
        if (current_depth - bias < closest_depth)
          shadow = 0.0;
      }
      
      u_map += 1.0 / 6.0;
    }
    
    v_map += 1.0 / float(MAX_LIGHTS);
    
    vec3 view_dir = normalize(u_view_pos - vs_pos);
    vec3 reflect_dir = reflect(-light_dir, normal);
    
    float specular = 0.4 * pow(max(dot(view_dir, reflect_dir), 0.0), 32.0);
    float diffuse = max(dot(normal, light_dir), 0.0);
    float delta_dist = length(delta_pos);
    
    float attentuation = lights[i].intensity / (1.0 + 4.0 * delta_dist + 0.4 * delta_dist * delta_dist);
    float intensity = (diffuse + specular) * attentuation;
    light += lights[i].color.xyz * intensity * (1.0 - shadow);
  }
  
  light += vec3(0.1, 0.1, 0.1);
  
  frag_color = texture(u_color, vs_uv) * vec4(light, 1.0);
}
