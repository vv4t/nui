#ifndef TEMPLATE_SHADER_H
#define TEMPLATE_SHADER_H

class shader_builder_t {
  
};

shader_t create_raw_shader(const char* src_vertex, const char *src_fragment);
shader_t create_surface_shader(const char* src_fragment);

#endif
