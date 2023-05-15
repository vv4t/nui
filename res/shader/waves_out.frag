out vec4 frag_color;

in vec2 vs_uv;

uniform sampler2D u_wave;

float u_at(vec2 x)
{
  return (texture(u_wave, x).r - 0.5) * 4.0;
}

void main()
{       
  vec2 d = 1.0 / vec2(textureSize(u_wave, 0));
  
  vec2 dx = vs_uv - vec2(d.x, 0);
  vec2 dy = vs_uv - vec2(0, d.y);
  
  float u = u_at(vs_uv);
  vec3 u1 = vec3(d.x, 0, u_at(dx) - u);
  vec3 u2 = vec3(0, d.y, u_at(dy) - u);
  
  vec3 n = (normalize(cross(u1, u2)) + 1.0) * 0.5;
  
  frag_color = vec4(n, 1.0);
}
