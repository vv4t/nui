#ifndef DEFER_H
#define DEFER_H

#include <stdbool.h>

bool defer_init();
void defer_begin();
void defer_end();
void defer_bind();

#endif
