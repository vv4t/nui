layout (location = 0) out vec4 wave_out;
layout (location = 1) out vec4 wave_normal;

uniform sampler2D wave_in;

in vec2 vs_uv;

void main()
{
  ivec2 uv = ivec2(vs_uv * vec2(textureSize(wave_in, 0)));
  
  vec2 p = texelFetch(wave_in, uv, 0).xy;
  
  float u = p.r;
  float u_t = p.g;
  
  float du_dx_1 = u - texelFetch(wave_in, ivec2(uv.x - 1, uv.y), 0).x;
  float du_dy_1 = u - texelFetch(wave_in, ivec2(uv.x, uv.y - 1), 0).x;
  
  float du_dx_2 = texelFetch(wave_in, ivec2(uv.x + 1, uv.y), 0).x - u;
  float du_dy_2 = texelFetch(wave_in, ivec2(uv.x, uv.y + 1), 0).x - u;
  
  float d2u_dx2 = du_dx_2 - du_dx_1;
  float d2u_dy2 = du_dy_2 - du_dy_1;
  
  float d2u_dt2 = d2u_dx2 + d2u_dy2;
  
  float c = 0.4;
  
  float u_0 = u * 2.0 - u_t + c * d2u_dt2;
  
  float s = 8.0;
  vec3 u1 = vec3(s, 0.0, du_dx_2);
  vec3 u2 = vec3(0.0, s, du_dy_2);
  
  vec3 normal = normalize(cross(u1, u2));
  
  wave_out = vec4(u_0, u, 1.0, 1.0);
  wave_normal = vec4((normal + 1.0) * 0.5, 1.0);
}
