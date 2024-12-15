#pragma use "camera.glsl"

layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec3 v_tangent;
layout(location = 3) in vec3 v_bitangent;
layout(location = 4) in vec2 v_uv;

out vec2 vs_uv;
out vec3 vs_pos;
out mat3 vs_TBN;

vec3 get_axis(vec3 N) {
  if (abs(N.y) > abs(N.x) && abs(N.y) > abs(N.z)) {
    return vec3(0.0, 0.0, 1.0);
  } else {
    return vec3(0.0, 1.0, 0.0);
  }
}

void main()
{
  vs_TBN = mat3(v_tangent, v_bitangent, v_normal);
  vs_pos = (model * vec4(v_pos, 1.0)).xyz;
  vs_uv = (inverse(vs_TBN) * vs_pos).xy * 0.75;

  gl_Position = MVP * vec4(v_pos, 1.0);
}
