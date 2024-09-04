layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec2 v_uv;

out vec2 vs_uv;

layout(std140) uniform ub_camera {
  vec3 plus;
};

void main()
{
  vs_uv = v_uv;
  gl_Position = vec4(v_pos + plus, 1.0);
}
