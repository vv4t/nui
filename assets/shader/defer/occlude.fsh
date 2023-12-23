out vec4 frag_color;

in vec2 vs_uv;

uniform sampler2D u_pos;
uniform sampler2D u_normal;
uniform sampler2D u_albedo;

layout (std140) uniform ubo_matrices {
  mat4 mvp;
  mat4 model;
  mat4 view_project;
  vec3 view_pos;
};

uniform vec3 u_samples[64];

vec3 get_frag_pos()
{
  return texture(u_pos, vs_uv).rgb;
}

vec3 get_frag_normal()
{
  return texture(u_normal, vs_uv).rgb;
}

vec4 get_diffuse()
{
  return texture(u_albedo, vs_uv);
}

float rand()
{
  return fract(sin(dot(vs_uv, vec2(12.9898, 78.233))) * 43758.5453);
}

float calc_occlusion()
{
  vec3 normal = get_frag_normal();
  vec3 some_vec = normal + vec3(rand(), rand() + 1.0, rand() + 2.0);
  vec3 tangent = normalize(some_vec - normal * dot(some_vec, normal));
  vec3 bitangent = cross(tangent, normal);
  mat3 TBN = mat3(tangent, bitangent, normal);
  
  vec4 frag_pos = view_project * vec4(get_frag_pos(), 1.0);
  
  float occlusion = 0.0;
  float radius = 0.5;
  
  for (int i = 0; i < 64; i++) {
    vec3 sample_pos = get_frag_pos() + (TBN * u_samples[i]) * radius;
    vec4 offset = view_project * vec4(sample_pos, 1.0);
    vec2 screen_pos = (offset.xy / offset.w) * 0.5 + 0.5;
    
    if (screen_pos.x < 0.0 || screen_pos.y < 0.0 || screen_pos.x > 1.0 || screen_pos.y > 1.0 || offset.z < 0.0) {
      continue;
    }
    
    float sample_depth = (view_project * texture(u_pos, screen_pos)).z;
    
    float range_check = smoothstep(0.0, 1.0, radius / abs(offset.z - sample_depth));
    occlusion += (sample_depth < offset.z + 0.025 ? 1.0 : 0.0);
  }
  
  return 1.0 - occlusion / 64.0;
}

void main()
{
  frag_color = vec4(vec3(1.0) * calc_occlusion(), 1.0);
}
