out vec4 frag_color;

in vec2 vs_uv;

uniform sampler2D u_wave;

float u_at(vec2 x)
{
  return texture(u_wave, x).r - 0.5;
}

void main()
{       
  vec2 d = 1.0 / vec2(textureSize(u_wave, 0));
  
  vec2 b = texture(u_wave, vs_uv).rg;
  
  float u = b.r - 0.5;
  float u_t = b.g - 0.5;
  
  float du_dx_1 = u - u_at(vs_uv - vec2(d.x, 0));
  float du_dy_1 = u - u_at(vs_uv - vec2(0, d.y));
  
  float du_dx_2 = u_at(vs_uv + vec2(d.x, 0)) - u;
  float du_dy_2 = u_at(vs_uv + vec2(0, d.y)) - u;
  
  float d2u_dx2 = du_dx_2 - du_dx_1;
  float d2u_dy2 = du_dy_2 - du_dy_1;
  
  float u_t_1 = u_t + 200.0 * (d2u_dx2 + d2u_dy2) * 0.05;
  float u_1 = u + u_t_1 * 0.05;
  
  frag_color = vec4(u_1 + 0.5, u_t_1 + 0.5, 0.0, 1.0);
}
