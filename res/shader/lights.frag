#version 300 es
precision mediump float;

layout (location = 0) out vec4 frag_color;

in vec4 vs_light_pos;
in vec3 vs_pos;
in vec3 vs_normal;
in vec2 vs_uv;
in mat3 vs_TBN;

uniform sampler2D u_color;
uniform sampler2D u_normal;
uniform sampler2D u_depth_map;

struct light_t {
  mat4  light_matrix;
  vec3  pos;
  float intensity;
  vec4  color;
};

layout (std140) uniform ubo_lights {
  light_t lights[8];
};

layout (std140) uniform ubo_matrices {
  mat4  mvp;
  mat4  model;
  vec3  view_pos;
  float pad[1];
};

void main() {
  vec3 light = vec3(0.0);
  
  vec3 normal = texture(u_normal, vs_uv).rgb;
  normal = normal * 2.0 - 1.0;
  normal = normalize(vs_TBN * normal);
  
  vec3 proj_coords = vs_light_pos.xyz / vs_light_pos.w;
  proj_coords = proj_coords * 0.5 + 0.5; 
  
  float closest_depth = texture2D(u_depth_map, proj_coords.xy).r;
  float current_depth = proj_coords.z;
  
  // float cos_theta = dot(vs_normal, normalize(-l));
  float bias = 0.005; // clamp(0.05 * (1.0 - cos_theta), 0.0, 0.005);
  float shadow = current_depth - bias > closest_depth ? 1.0 : 0.0;
  
  if (
    proj_coords.x > 1.0 || 
    proj_coords.x < 0.0 || 
    proj_coords.y > 1.0 || 
    proj_coords.y < 0.0 || 
    proj_coords.z > 1.0 ||
    proj_coords.z < 0.0
  )
    shadow = 1.0;
  
  for (int i = 0; i < 8; i++) {
    if (lights[i].intensity <= 0.0)
      continue;
    
    vec3 delta_pos = lights[i].pos - vs_pos;
    vec3 view_dir = normalize(view_pos - vs_pos);
    vec3 light_dir = normalize(delta_pos);
    vec3 reflect_dir = reflect(-light_dir, normal);
    
    float specular = 2.5 * pow(max(dot(view_dir, reflect_dir), 0.0), 32.0);
    float diffuse = max(dot(normal, light_dir), 0.0);
    float delta_dist = length(delta_pos);
    
    float attentuation = lights[i].intensity / (1.0 + 4.0 * delta_dist + 0.4 * delta_dist * delta_dist);
    float intensity = (diffuse + specular) * attentuation;
    light += lights[i].color.xyz * intensity;
  }
  
  light *= (1.0 - shadow);
  light += vec3(0.3, 0.3, 0.3);
  
  frag_color = texture(u_color, vs_uv) * vec4(light, 1.0);
}
