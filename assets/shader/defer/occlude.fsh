uniform vec3 u_samples[64];

float rand()
{
  return fract(sin(dot(vs_uv, vec2(12.9898, 78.233))) * 43758.5453);
}

float calc_occlusion()
{
  vec3 normal = get_frag_normal();
  
  vec3 rand_vec = normal + vec3(rand(), rand() + 1.0, rand() + 2.0);
  
  vec3 tangent = normalize(rand_vec - normal * dot(rand_vec, normal));
  vec3 bitangent = cross(tangent, normal);
  mat3 TBN = mat3(tangent, bitangent, normal);
  
  float occlusion = 0.0;
  
  float radius = 0.5;
  float bias = 0.03;
  
  for (int i = 0; i < 64; i++) {
    vec3 sample_pos = get_world_pos() + (TBN * u_samples[i]) * radius;
    vec4 offset = mat_mvp * vec4(sample_pos, 1.0);
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

void frag_pass()
{
  set_frag(vec4(vec3(1.0) * calc_occlusion(), 1.0));
}
