#pragma use "camera.glsl"
#pragma use "PBR.glsl"

in vec2 vs_uv;

out vec4 frag_color;

uniform sampler2D u_depth;
uniform sampler2D u_normal;
uniform sampler2D u_radiance;

uniform float g_time;

float rand(vec2 co) {
  return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

float f(vec2 p) {
  float z = 0.0;

  for (float i = 0.0; i < 10.0; i += 1.0) {
    float x = rand(vec2(i, i * 10.0));
    float y = rand(vec2(i, x * 1000.0));
    vec2 q = (vec2(x, y) - 0.5) * 2.0 * 10.0;
    z += cos((length(p - q) + g_time * x * y * 8.0) * 10.0) * y;
  }
  
  return z * 0.05;
}

vec3 calculate_water_normal(vec2 p) {
  vec2 p_x = p + vec2(0.05, 0.0);
  vec2 p_y = p + vec2(0.0, 0.05);
  
  float df_dx = f(p_x) - f(p);
  float df_dy = f(p_y) - f(p);
  
  return normalize(vec3(df_dx, df_dy, 1.0)).xyz;
}

void main() {
  float near = 0.1;
  float far = 100.0;
  float z_scale = (-far + -near) / (-far - -near);
  float z_offset = (2.0 * -far * -near) / (-far - -near);

  float depth = texture(u_depth, vs_uv).z;
  float z = z_offset / (depth * 2.0 - 1.0 - z_scale);
  vec3 frag_pos = vec3((vs_uv * 2.0 - 1.0) * z, z);

  vec3 rd = normalize(vec3(vs_uv * 2.0 - 1.0, 1.0) * mat3(view));
  vec3 ro = view_pos;

  vec3 n = normalize(vec3(0.0, 1.0, 0.0));
  float d = 1.25;
  float td = dot(n * d - ro, n) / dot(rd, n);
  vec3 p = view_pos + rd * td;
  
  vec3 color = texture(u_radiance, vs_uv).xyz;

  mat3 inv_view = transpose(mat3(view));

  if (
    td > 0.0 && td < length(frag_pos)
  ) {
    vec3 V = normalize(frag_pos);
    vec3 N = calculate_water_normal(p.xz).xzy * inv_view;

    vec3 new_pos = V * td;
    vec3 R = refract(V, N, 1.0 / 1.33);
    vec3 diffuse = color;
    
    for (int i = 0; i < 128; i++) {
      new_pos += R / 32.0 * (new_pos.z + R.z / 32.0);

      vec2 uv = new_pos.xy / new_pos.z * 0.5 + 0.5;
      depth = texture(u_depth, uv).z;
      z = z_offset / (depth * 2.0 - 1.0 - z_scale);

      if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0 || new_pos.z < 0.1) break;

      if (new_pos.z > z + 0.01 && new_pos.z < z + 1.0) {
        diffuse = texture(u_radiance, uv).xyz;
        break;
      }
    }

    diffuse *= vec3(0.6, 0.9, 1.0);
    
    new_pos = V * td;
    R = reflect(V, N);
    vec3 specular = vec3(0.0);

    for (int i = 0; i < 128; i++) {
      new_pos += R / 32.0 * (new_pos.z + R.z / 32.0);

      vec2 uv = new_pos.xy / new_pos.z * 0.5 + 0.5;
      depth = texture(u_depth, uv).z;
      z = z_offset / (depth * 2.0 - 1.0 - z_scale);

      if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0 || new_pos.z < 0.1) break;

      if (new_pos.z > z + 0.01 && new_pos.z < z + 1.0) {
        specular = texture(u_radiance, uv).xyz * pow(0.94, float(i));
        break;
      }
    }
    
    vec3 L = normalize(new_pos - frag_pos);
    vec3 H = normalize(L + V);
    vec3 F0 = vec3(0.04);
    vec3 kS = fresnelSchlick(max(dot(H, V), 0.0), F0);
    vec3 kD = vec3(1.0) - kS;

    color = kD * diffuse / M_PI + specular * kD;
  }

  frag_color = vec4(color, 1.0);
}
