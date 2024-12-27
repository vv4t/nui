#pragma use "camera.glsl"

in vec2 vs_uv;

out vec4 frag_color;

uniform sampler2D u_depth;
uniform sampler2D u_normal;
uniform sampler2D u_radiance;
uniform sampler2D u_water_normal;

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
  float d = 0.52;
  float td = dot(n * d - ro, n) / dot(rd, n);
  vec3 p = view_pos + rd * td;
  
  vec3 color = texture(u_radiance, vs_uv).xyz;

  mat3 inv_view = transpose(mat3(view));

  if (
    td > 0.0 && td < length(frag_pos)
  ) {
    vec3 V = normalize(frag_pos);
    vec3 N = (texture(u_water_normal, p.xz * 0.25 + view_pos.xz * 0.125).xzy * 2.0 - 1.0) * inv_view;
    vec3 R = refract(V, N, 1.0 / 1.33);

    frag_pos = V * td;

    for (int i = 0; i < 128; i++) {
      frag_pos += R / 32.0 * (frag_pos.z + R.z / 32.0);

      vec2 uv = frag_pos.xy / frag_pos.z * 0.5 + 0.5;
      depth = texture(u_depth, uv).z;
      z = z_offset / (depth * 2.0 - 1.0 - z_scale);

      if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0 || frag_pos.z < 0.1) break;

      if (frag_pos.z > z + 0.01 && frag_pos.z < z + 0.05) {
        color = texture(u_radiance, uv).xyz;
        break;
      }
    }

    color *= vec3(0.7, 0.9, 1.0);
  }

  frag_color = vec4(color, 1.0);
}
