layout (std140) uniform ub_camera {
  mat4 mat_mvp;
  mat4 mat_model;
  mat4 mat_view;
  mat4 mat_look;
  vec3 view_pos;
};

#define get_mvp() mat_mvp
#define get_model() mat_model
#define get_view() mat_view
#define get_look() mat_look
#define get_view_pos() view_pos
