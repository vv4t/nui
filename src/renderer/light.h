#ifndef LIGHT_H
#define LIGHT_H

bool light_init();
void light_bind();
void light_add_point(vec3_t pos, vec3_t color, float intensity);
void light_reset();

#endif
