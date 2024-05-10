layout(std140) uniform ub_camera {
  mat4 model;
  mat4 view;
  mat4 project;
  mat4 mvp;
  vec3 view_pos;
};
