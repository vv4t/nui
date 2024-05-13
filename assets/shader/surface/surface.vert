layout(location = 0) in vec3 v_p;
layout(location = 1) in vec3 v_n;
layout(location = 2) in vec3 v_t;
layout(location = 3) in vec3 v_bt;
layout(location = 4) in vec2 v_uv;

out vec2 vs_uv;
out vec3 vs_frag_pos;
out vec3 vs_world_pos;
out mat3 vs_TBN;

void main()
{
  mat3 TBN = mat3(transpose(inverse(model))) * mat3(v_t, v_bt, v_n);
  vs_TBN = mat3(
    normalize(TBN[0]),
    normalize(TBN[1]),
    normalize(TBN[2])
  );
  
  vs_world_pos = vec3(model * vec4(v_p, 1.0));
  
#ifdef PLANAR_UV
  vs_uv = (inverse(vs_TBN) * vs_world_pos * 0.5).xy;
#else
  vs_uv = vec2(v_uv.x, 1.0 - v_uv.y);
#endif
  
  gl_Position = mvp * vec4(v_p, 1.0);
  vs_frag_pos = gl_Position.xyz;
}
