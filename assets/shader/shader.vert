layout(location = 0) in vec3 v_p;

layout(std140) uniform ub_camera {
  mat4 mvp;
};

void main()
{
  gl_Position = mvp * vec4(v_p, 1.0);
}
