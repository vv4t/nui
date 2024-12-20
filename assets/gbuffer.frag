layout (location = 0) out vec4 g_radiance;
layout (location = 1) out vec4 g_normal;

in vec2 vs_uv;
in vec3 vs_pos;
in mat3 vs_TBN;

uniform sampler2D u_albedo;
uniform sampler2D u_normal;

void main() {
  g_radiance = texture(u_albedo, vs_uv);
  g_normal = texture(u_normal, vs_uv);
}
