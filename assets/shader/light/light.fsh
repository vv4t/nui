out vec4 frag_color;

in vec3 vs_pos;
in vec2 vs_uv;

uniform sampler2D u_color;

void main()
{
  vec3 light_pos = vec3(0.0, 1.0, 0.0);
  vec3 light_color = vec3(1.0, 1.0, 1.0);
  float light_intensity = 10.0;
  
  vec3 delta_pos = light_pos - vs_pos;
  float delta_dist = length(delta_pos);
  
  float attentuation = light_intensity / (1.0 + 4.0 * delta_dist + 0.4 * delta_dist * delta_dist);
  
  vec3 light = light_color * attentuation;
  
  frag_color = vec4(light, 1.0) * texture(u_color, vs_uv);
}
