out vec4 frag_color;

#define POINT_MAX 8
#define AMBIENCE 0.05

struct ub_point_t {
  vec4 position;
  vec4 color;
};

layout (std140) uniform ub_light {
  ub_point_t point[POINT_MAX];
};

void surface_shade(
  in vec3 frag_pos,
  vec2 uv,
  vec3 normal,
  sampler2D albedo
) {
  vec3 light = vec3(0.0);
  
  for (int i = 0; i < POINT_MAX; i++) {
    vec3 delta_pos = point[i].position.xyz - frag_pos;
    float delta_dist = length(delta_pos);
    float diffuse = clamp(dot(normal, normalize(delta_pos)), 0.0, 1.0);
    float intensity = point[i].color.w / (delta_dist * delta_dist + delta_dist);
    float alpha = intensity * diffuse;
    
    light += point[i].color.xyz * alpha;
  }
  
  light += AMBIENCE;
  
  frag_color = vec4(light.xyz * texture(albedo, uv).xyz, 1.0);
}
