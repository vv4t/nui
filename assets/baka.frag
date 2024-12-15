#pragma use "camera.glsl"
#pragma use "lighting.glsl"

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
  vec3 normal = vs_TBN * (texture(u_normal, vs_uv).xyz - 0.5) * 2.0;

  for (int i = 0; i < MAX_LIGHTS; i++) {
    vec3 delta_light_frag = lights[i].position - vs_pos;
    vec3 delta_view_frag = view_pos - vs_pos;
    
    vec3 light_dir = normalize(delta_light_frag);
    vec3 view_dir = normalize(delta_view_frag);
    vec3 halfway_dir = normalize(light_dir + view_dir);
    
    float diffuse = max(dot(light_dir, normal), 0.0);
    float specular = pow(max(dot(normal, halfway_dir), 0.0), 4.0);
    float attenuation = 1.0 / dot(delta_light_frag, delta_light_frag);

    light += lights[i].radiance * (diffuse + specular) * attenuation;
  }

  light *= albedo;

  frag_color = vec4(light, 1.0);
}
