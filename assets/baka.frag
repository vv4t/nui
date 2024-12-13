#pragma use "camera.glsl"

in vec2 vs_uv;
in vec3 vs_pos;
in vec3 vs_normal;

out vec4 frag_color;

uniform sampler2D u_texture;

void main()
{
  frag_color = texture(u_texture, vs_uv) / length(vs_pos - view_pos);
}
