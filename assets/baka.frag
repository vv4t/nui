#pragma use "camera.glsl"
#pragma use "lighting.glsl"

in vec2 vs_uv;
in vec3 vs_pos;
in vec3 vs_normal;

out vec4 frag_color;

uniform sampler2D u_texture;

void main()
{
  vec3 light = vec3(0.0);

  for (int i = 0; i < MAX_LIGHTS; i++) {
    vec3 delta_light_frag = lights[i].position - vs_pos;
    vec3 delta_view_frag = view_pos - vs_pos;
    
    vec3 light_dir = normalize(delta_light_frag);
    vec3 view_dir = normalize(delta_view_frag);
    vec3 halfway_dir = normalize(light_dir + view_dir);
    
    float diffuse = max(dot(light_dir, vs_normal), 0.0);
    float specular = pow(max(dot(vs_normal, halfway_dir), 0.0), 4.0);
    float attenuation = 1.0 / dot(delta_light_frag, delta_light_frag);

    light += lights[i].radiance * (diffuse + specular) * attenuation;
  }

  light *= texture(u_texture, vs_uv).xyz;

  frag_color = vec4(light, 1.0);
}
