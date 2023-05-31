out vec4 frag_color;

in vec2 vs_uv;

uniform sampler2D u_wave;

void main()
{       
  vec2 d = 1.0 / vec2(textureSize(u_wave, 0));
  
  vec2 dx = vs_uv + vec2(d.x, 0);
  vec2 dy = vs_uv + vec2(0, d.y);
  
  vec4 u = (texture(u_wave, vs_uv) - 0.5) * 2.0;
  vec3 u1 = vec3(1.0, 0, u.b);
  vec3 u2 = vec3(0, 1.0, u.a);
  
  vec3 n = (normalize(cross(u1, u2)) + 1.0) * 0.5;
  
  frag_color = vec4(n, 1.0);
}
