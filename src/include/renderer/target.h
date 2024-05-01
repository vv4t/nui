#ifndef TARGET_H
#define TARGET_H

#include <GL/glew.h>

typedef GLuint target_t;

target_t target_create(int num_bindings, ...);
void target_bind(target_t target);
void target_unbind();
void target_destroy(target_t target);

#endif
