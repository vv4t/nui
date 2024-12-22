in vec2 vs_uv;

out vec4 frag_color;

uniform sampler2D u_depth;
uniform sampler2D u_normal;
uniform sampler2D u_radiance;

uniform vec3 u_samples[32];

void main() {
  float near = 0.1;
  float far = 100.0;
  float z_scale = (-far + -near) / (-far - -near);
  float z_offset = (2.0 * -far * -near) / (-far - -near);

  float depth = texture(u_depth, vs_uv).z;
  float z = z_offset / (depth - z_scale);
  vec3 frag_pos = vec3((vs_uv * 2.0 - 1.0) * z, z);

  vec3 normal = texture(u_normal, vs_uv).xyz;

  vec3 new_dir = normal + normal.zyx + normal.yzx;
  vec3 tangent = normalize(new_dir - normal * dot(new_dir, normal));
  vec3 bitangent = cross(tangent, normal);
  mat3 TBN = mat3(tangent, bitangent, normal);

  float occlusion = 0.0;
  
  float radius = 0.25;
  float bias = 0.03;
  
  for (int i = 0; i < 32; i++) {
    vec3 sample_pos = frag_pos + (TBN * u_samples[i]) * radius;
    vec2 screen_pos = (sample_pos.xy / sample_pos.z) * 0.5 + 0.5;
    
    if (
      screen_pos.x < 0.0 || screen_pos.y < 0.0 ||
      screen_pos.x > 1.0 || screen_pos.y > 1.0 ||
      sample_pos.z < 0.0
    ) {
      continue;
    }
    
    float sample_depth = texture(u_depth, screen_pos).z;
    sample_depth = z_offset / (sample_depth - z_scale);
    
    float range_check = smoothstep(0.0, 1.0, radius / abs(sample_pos.z - sample_depth));
    occlusion += (sample_depth + bias < sample_pos.z ? 1.0 : 0.0) * range_check;
  }

  occlusion = pow(1.0 - occlusion / 32.0, 4.0);

  vec3 color = texture(u_radiance, vs_uv).xyz * occlusion;

  frag_color = vec4(color, 1.0);
}
