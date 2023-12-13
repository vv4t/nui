#define CUBE_FACES 6

out vec4 frag_color;

in vec2 vs_uv;

uniform sampler2D u_pos;
uniform sampler2D u_normal;
uniform sampler2D u_albedo;

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

layout (std140) uniform ubo_matrices {
  mat4 mvp;
  mat4 model;
  mat4 view_project;
  vec3 view_pos;
};

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

in vec4 vs_light_pos[CUBE_FACES * POINTS_MAX];

uniform sampler2D u_depth_map;
uniform vec3 u_samples[64];

float rand()
{
  return fract(sin(dot(vs_uv, vec2(12.9898, 78.233))) * 43758.5453);
}

float calc_point_shadow_face(int id, int face, vec3 light_dir, vec3 normal)
{
  vec4 shadow_pos = point_shadows[id].light_matrices[face] * vec4(get_frag_pos(), 1.0);
  vec3 shadow_coord = shadow_pos.xyz / shadow_pos.w * 0.5 + vec3(0.5, 0.5, 0.5);
  
  if (shadow_coord.z < 0.0 || shadow_coord.z > 1.0) {
    return 0.0;
  }
  
  vec2 poisson_disk[4] = vec2[](
    vec2( -0.94201624, -0.39906216 ),
    vec2( 0.94558609, -0.76890725 ),
    vec2( -0.094184101, -0.92938870 ),
    vec2( 0.34495938, 0.29387760 )
  );
  
  float shadow = 0.0;
  
  for (int k = 0; k < 4; k++) {
    vec2 shadow_uv = shadow_coord.xy;
    
    if (shadow_uv.x > 0.0 && shadow_uv.x < 1.0 && shadow_uv.y > 0.0 && shadow_uv.y < 1.0) {
      shadow_uv += poisson_disk[k] / 700.0;
      
      shadow_uv.x = clamp(shadow_uv.x, 0.01, 0.99);
      shadow_uv.y = clamp(shadow_uv.y, 0.01, 0.99);
      
      shadow_uv.x = (shadow_uv.x + float(face)) / float(CUBE_FACES);
      shadow_uv.y = (shadow_uv.y + float(id)) / float(POINTS_MAX);
      
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
  
  return shadow;
}

float calc_point_shadow(int id, vec3 light_dir, vec3 normal)
{
  float shadow = 0.0;
  
  // https://gist.github.com/hypernewbie/f80958de2cb262f8de2d00e52e1a0052
  
	vec3 v = abs(light_dir);
  int face_id = 0;
  
	if(v.z >= v.x && v.z >= v.y) {
		face_id = (light_dir.z < 0.0) ? 4 : 5;
	} else if(v.y >= v.x) {
		face_id = light_dir.y < 0.0 ? 2 : 3;
	} else {
		face_id = light_dir.x < 0.0 ? 0 : 1;
	}
  
  shadow += calc_point_shadow_face(id, face_id, light_dir, normal);
  
  return shadow;
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
    occlusion += (sample_depth < offset.z + 0.025 ? 1.0 : 0.0) * range_check;
  }
  
  return 1.0 - occlusion / 64.0;
}

vec3 calc_light(int id)
{
  vec3 frag_pos = get_frag_pos();
  vec3 frag_normal = get_frag_normal();
  
  vec3 delta_pos = points[id].pos - frag_pos;
  vec3 light_dir = normalize(delta_pos);
  
  vec3 view_dir = normalize(view_pos - frag_pos);
  vec3 reflect_dir = reflect(-light_dir, frag_normal);
  
  float specular = 0.4 * pow(max(dot(view_dir, reflect_dir), 0.0), 32.0);
  float diffuse = max(dot(frag_normal, light_dir), 0.0);
  float delta_dist = length(delta_pos);
  
  float attenuation = points[id].intensity / (1.0 + 4.0 * delta_dist + 0.4 * delta_dist * delta_dist);
  float intensity = (diffuse + specular) * attenuation;
  
  return points[id].color.xyz * intensity * (1.0 - calc_point_shadow(id, light_dir, frag_normal));
}

void main()
{
  vec3 light = vec3(0.0);
  
  for (int i = 0; i < POINTS_MAX; i++) {
    if (points[i].intensity <= 0.0) {
      continue;
    }
    
    light += calc_light(i);
  }
  
  light += vec3(0.1, 0.1, 0.1);
  light *= calc_occlusion();
  
  frag_color = vec4(vec3(1.0) * light, 1.0);
}
