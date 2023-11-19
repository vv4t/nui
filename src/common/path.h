#ifndef PATH_H
#define PATH_H

#define PATH_MAX 256

typedef char path_t[PATH_MAX];

void path_create(path_t path, const char *format, ...);
void path_new(path_t path, const char *file);
void path_copy(path_t dest, const path_t src);

#endif
