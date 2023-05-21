out vec4 frag_color;

in vec2 vs_uv;

uniform sampler2D u_wave;

float u_at(ivec2 x)
{
  return texelFetch(u_wave, x, 0).r - 0.5;
}

void main()
{
  ivec2 uv = ivec2(vs_uv * vec2(textureSize(u_wave, 0)));
  
  vec2 b = texelFetch(u_wave, uv, 0).rg;
  
  float u = b.r - 0.5;
  float u_t = b.g - 0.5;
  
  float du_dx_1 = u - u_at(ivec2(uv.x - 1, uv.y));
  float du_dy_1 = u - u_at(ivec2(uv.x, uv.y - 1));
  
  float du_dx_2 = u_at(ivec2(uv.x + 1, uv.y)) - u;
  float du_dy_2 = u_at(ivec2(uv.x, uv.y + 1)) - u;
  
  float d2u_dx2 = du_dx_2 - du_dx_1;
  float d2u_dy2 = du_dy_2 - du_dy_1;
  
  float h = 1.0;
  float k = 1.0;
  
  float c = 0.8;
  
  float tau = pow(((c*k)/h), 2.0);
  
  float d2u_dt2 = d2u_dx2 + d2u_dy2;
  
  float u_0 = u * 2.0 - u_t + tau * d2u_dt2;
  
  frag_color = vec4(u_0 * 0.995 + 0.5, u + 0.5, du_dx_2 + 0.5, du_dy_2 + 0.5);
}
