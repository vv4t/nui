layout(location = 0) in vec3 v_p;

out vec3 vs_pos;

void main()
{
  vs_pos = v_p;
  gl_Position = project * mat4(mat3(view)) * vec4(v_p, 1.0);
}
