#ifndef PIPELINE_H
#define PIPELINE_H

#include <stdbool.h>

typedef struct {
  bool (*init)();
  void (*setup)();
  void (*pass)();
} pipeline_t;

extern pipeline_t pipeline_chise;
extern pipeline_t pipeline_misaki;
extern pipeline_t pipeline_akarin;
extern pipeline_t pipeline_reflect;
extern pipeline_t pipeline_moom;

#endif
