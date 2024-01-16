#define NUM_SAMPLES 64

uniform vec3 u_samples[NUM_SAMPLES];

float calc_ssao(sampler2D u_pos, vec3 frag_pos, vec3 normal)
{
  vec3 new_dir = normal + normal.zyx;
  
  vec3 tangent = normalize(new_dir - normal * dot(new_dir, normal));
  vec3 bitangent = cross(tangent, normal);
  mat3 TBN = mat3(tangent, bitangent, normal);
  
  float occlusion = 0.0;
  
  float radius = 0.5;
  float bias = 0.03;
  
  for (int i = 0; i < 64; i++) {
    vec3 sample_pos = frag_pos + (TBN * u_samples[i]) * radius;
    vec4 offset = get_mvp() * vec4(sample_pos, 1.0);
    vec2 screen_pos = (offset.xy / offset.w) * 0.5 + 0.5;
    
    if (
      screen_pos.x < 0.0 || screen_pos.y < 0.0 ||
      screen_pos.x > 1.0 || screen_pos.y > 1.0 ||
      offset.z < 0.0
    ) {
      continue;
    }
    
    float sample_depth = texture(u_pos, screen_pos).z;
    
    float range_check = smoothstep(0.0, 1.0, radius / abs(offset.z - sample_depth));
    occlusion += (sample_depth + bias < offset.z ? 1.0 : 0.0) * range_check;
  }
  
  return 1.0 - occlusion / 64.0;
}
