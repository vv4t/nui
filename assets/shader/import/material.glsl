uniform sampler2D u_color;
uniform sampler2D u_normal;

layout (std140) uniform ub_material {
  vec3 m_color;
  float m_specular;
};

#define get_color() vec4(m_color, 1.0)
#define get_specular() m_specular
#define get_diffuse() (texture(u_color, vs_uv) * get_color())
