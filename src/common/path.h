#ifndef PATH_H
#define PATH_H

#define PATH_LEN 256

typedef struct {
  char base[PATH_LEN];
  char name[PATH_LEN];
} path_t;

void path_create(path_t *path, const char *type, const char *name);
void path_join(path_t *path, char path_name[PATH_LEN], const char *name);

#endif
