#include "path.h"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <libgen.h>

void path_create(path_t path, const char *format, ...)
{
  va_list args;
  va_start(args, format);
  vsnprintf(path, PATH_MAX, format, args);
  va_end(args);
}

void path_new(path_t path, const char *file)
{
  path_t tmp;
  strncpy(tmp, path, PATH_MAX);
  snprintf(path, PATH_MAX, "%s/%s", dirname(tmp), file);
}

void path_copy(path_t dest, const path_t src)
{
  strncpy(dest, src, PATH_MAX);
}
