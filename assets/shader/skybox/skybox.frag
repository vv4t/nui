out vec4 frag_color;

in vec3 vs_pos;

uniform samplerCube u_cubemap;

void main()
{
  frag_color = textureCube(u_cubemap, vs_pos);
  
  for (int i = 0; i < POINT_MAX; i++) {
    frag_color.xyz += calc_point_scatter(i, vs_pos * 1000.0, view_pos);
  }
}
