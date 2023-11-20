#define MAX_LIGHTS 4

out vec4 frag_color;

struct light_t {
  vec3  pos;
  float intensity;
  vec4  color;
};

layout (std140) uniform ubo_lights {
  light_t lights[MAX_LIGHTS];
};

uniform vec3 u_view_pos;

in vec3 vs_pos;
in vec3 vs_normal;
in vec2 vs_uv;

uniform sampler2D u_color;

void main()
{
  vec3 light = vec3(0.0);
  
  vec3 normal = vs_normal;
  
  for (int i = 0; i < MAX_LIGHTS; i++) {
    if (lights[i].intensity <= 0.0)
      continue;
    
    vec3 delta_pos = lights[i].pos - vs_pos;
    vec3 light_dir = normalize(delta_pos);
    
    vec3 view_dir = normalize(u_view_pos - vs_pos);
    vec3 reflect_dir = reflect(-light_dir, normal);
    
    float specular = 0.4 * pow(max(dot(view_dir, reflect_dir), 0.0), 32.0);
    float diffuse = max(dot(normal, light_dir), 0.0);
    float delta_dist = length(delta_pos);
    
    float attenuation = lights[i].intensity / (1.0 + 4.0 * delta_dist + 0.4 * delta_dist * delta_dist);
    float intensity = (diffuse + specular) * attenuation;
    
    light += lights[i].color.xyz * intensity;
  }
  
  light += vec3(0.1, 0.1, 0.1);
  
  vec4 diffuse = texture(u_color, vs_uv);
  
  frag_color = diffuse * vec4(light, 1.0);
}
