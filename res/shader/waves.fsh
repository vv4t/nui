out vec4 frag_color;

in vec2 vs_uv;

uniform sampler2D u_wave;
uniform bool      u_compute;

float u_at(ivec2 x)
{
  return texelFetch(u_wave, x, 0).r - 0.5;
}

void main()
{
  if (u_compute) {
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
    
    float d2u_dt2 = d2u_dx2 + d2u_dy2;
    
    float h = 0.1;
    float k = 0.015;
    float c = 4.0;
    
    float tau = pow(((c*k)/h), 2.0);
    
    float u_0 = u * 2.0 - u_t + tau * d2u_dt2;
    
    frag_color = vec4(u_0 + 0.5, u + 0.5, du_dx_2 + 0.5, du_dy_2 + 0.5);
  } else {
    frag_color = texture(u_wave, vs_uv);
  }
}
