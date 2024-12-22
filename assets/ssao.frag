in vec2 vs_uv;

out vec4 frag_color;

uniform sampler2D u_depth;
uniform sampler2D u_normal;
uniform sampler2D u_radiance;

float rand(vec2 co) {
  return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
  float near = 0.1;
  float far = 100.0;
  float z_scale = (-far + -near) / (-far - -near);
  float z_offset = (2.0 * -far * -near) / (-far - -near);

  float depth = texture(u_depth, vs_uv).z;
  float z = z_offset / (depth - z_scale);

  vec3 color = texture(u_radiance, vs_uv).xyz;

  frag_color = vec4(color, 1.0);
}
