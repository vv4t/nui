#pragma use "camera.glsl"
#pragma use "lighting.glsl"
#pragma use "PBR.glsl"

in vec2 vs_uv;
in vec3 vs_pos;
in mat3 vs_TBN;

out vec4 frag_color;

uniform sampler2D u_albedo;
uniform sampler2D u_normal;

void main()
{
  vec3 light = vec3(0.0);

  vec3 albedo = texture(u_albedo, vs_uv).xyz;

  vec3 N = normalize(vs_TBN * (texture(u_normal, vs_uv).xyz - 0.5) * 2.0);
  vec3 V = normalize(view_pos - vs_pos);

  for (int i = 0; i < MAX_LIGHTS; i++) {
    if (lights[i].intensity <= 0.0) {
      continue;
    }

    vec3 delta_light_frag = lights[i].position - vs_pos;
    vec3 L = normalize(delta_light_frag);
    vec3 radiance = lights[i].radiance * lights[i].intensity;
    float attenuation = 1.0 / dot(delta_light_frag, delta_light_frag);

    light += radiance * attenuation * CookTorranceBRDF(albedo, 0.1, 0.7, L, V, N);
  }
  
  frag_color = vec4(light, 1.0);
}
