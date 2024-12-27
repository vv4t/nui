#pragma use "camera.glsl"

in vec2 vs_uv;

out vec4 frag_color;

uniform sampler2D u_depth;
uniform sampler2D u_normal;
uniform sampler2D u_radiance;

void main() {
  float near = 0.1;
  float far = 100.0;
  float z_scale = (-far + -near) / (-far - -near);
  float z_offset = (2.0 * -far * -near) / (-far - -near);

  float depth = texture(u_depth, vs_uv).z;
  float z = z_offset / (depth - z_scale);
  vec3 frag_pos = vec3((vs_uv * 2.0 - 1.0) * z, z);

  vec3 rd = normalize(vec3(vs_uv * 2.0 - 1.0, 1.0) * mat3(view));
  vec3 ro = view_pos;

  vec3 n = normalize(vec3(0.0, 1.0, 0.0));
  float d = 0.5;
  float td = dot(n * d - ro, n) / dot(rd, n);
  vec3 p = view_pos + rd * td;
  
  vec3 color = texture(u_radiance, vs_uv).xyz;

  if (
    td > 0.0 && td < length(frag_pos) * 0.5 &&
    p.x > 0.0 && p.x < 12.0 &&
    p.z > 0.0 && p.z < 12.0
  ) {
    color *= vec3(0.2, 0.2, 1.0);
  }

  frag_color = vec4(color, 1.0);
}
