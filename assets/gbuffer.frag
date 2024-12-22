#pragma use "camera.glsl"
#pragma use "lighting.glsl"
#pragma use "PBR.glsl"

layout (location = 0) out vec4 g_radiance;
layout (location = 1) out vec4 g_normal;

in vec2 vs_uv;
in vec3 vs_pos;
in mat3 vs_TBN;

uniform sampler2D u_albedo;
uniform sampler2D u_normal;

void main() {
  vec3 light = vec3(0.0);

  vec3 albedo = texture(u_albedo, vs_uv).xyz;

  vec3 N = normalize(vs_TBN * (texture(u_normal, vs_uv).xyz * 2.0 - 1.0));
  vec3 V = normalize(view_pos - vs_pos);

  for (int i = 0; i < MAX_LIGHTS; i++) {
    if (lights[i].intensity <= 0.0) {
      continue;
    }

    vec3 delta_light_frag = lights[i].position - vs_pos;
    vec3 L = normalize(delta_light_frag);
    float NdotL = max(dot(N, L), 0.0);
    vec3 radiance = lights[i].radiance * lights[i].intensity;
    float attenuation = 1.0 / dot(delta_light_frag, delta_light_frag);

    light += radiance * attenuation * CookTorranceBRDF(albedo, 0.1, 0.6, L, V, N) * NdotL;
  }

  g_radiance = vec4(light, 1.0);
  g_normal = vec4(normalize(vec3(view_project * vec4(vs_TBN[2], 0.0))), 1.0);
}
