#include "path.h"

#include <string.h>

void path_create(path_t *path, const char *type, const char *name)
{
  strncpy(path->base, "assets/", PATH_LEN - 1);
  strncat(path->base, type, PATH_LEN - 1);
  strncat(path->base, "/", PATH_LEN - 1);
  strncat(path->base, name, PATH_LEN - 1);
  strncat(path->base, "/", PATH_LEN - 1);
  
  strncpy(path->name, path->base, PATH_LEN - 1);
  strncat(path->name, name, PATH_LEN - 1);
  strncat(path->name, ".", PATH_LEN - 1);
  strncat(path->name, type, PATH_LEN - 1);
}

void path_join(path_t *path, char path_name[PATH_LEN], const char *name)
{
  strncpy(path_name, path->base, PATH_LEN);
  strncat(path_name, name, PATH_LEN);
}
