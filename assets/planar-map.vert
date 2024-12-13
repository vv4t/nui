#pragma use "camera.glsl"

layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_uv;

out vec2 vs_uv;
out vec3 vs_pos;
out vec3 vs_normal;

vec3 get_axis(vec3 N) {
  if (abs(N.y) > abs(N.x) && abs(N.y) > abs(N.z)) {
    return vec3(0.0, 0.0, 1.0);
  } else {
    return vec3(0.0, 1.0, 0.0);
  }
}

void main()
{
  vec3 N = normalize(mat3(transpose(inverse(model))) * v_normal);
  vec3 a = get_axis(N);
  vec3 T = normalize(cross(N, a));
  vec3 B = normalize(cross(N, T));

  mat3 TBN = mat3(T, B, N);
  
  vs_pos = (model * vec4(v_pos, 1.0)).xyz;
  vs_normal = N;
  vs_uv = (inverse(TBN) * vs_pos).xy * 0.5;

  gl_Position = MVP * vec4(v_pos, 1.0);
}
