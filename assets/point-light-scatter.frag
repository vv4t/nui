#pragma use "camera.glsl"
#pragma use "lighting.glsl"

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

  vec3 V = normalize(frag_pos);
  vec3 color = texture(u_radiance, vs_uv).xyz;

  mat4 inv_view = inverse(view);

  for (int i = 0; i < MAX_LIGHTS; i++) {
    if (lights[i].intensity <= 0.0) {
      continue;
    }

    vec3 light_pos = (vec4(lights[i].position - view_pos, 1.0) * inv_view).xyz;
    vec3 L = normalize(light_pos - frag_pos);
    vec3 dir = normalize(L - V * dot(L, V));
    float h = dot(light_pos, dir);
    float c = dot(light_pos, V);
    float a = -c;
    float b = dot(frag_pos, V) - c;
    float fog = atan(b / h) / h - atan(a / h) / h;
    
    color += lights[i].radiance * lights[i].intensity * (0.01 * fog * fog + 0.05 * fog) * 0.05;
  }

  frag_color = vec4(color, 1.0);
}
