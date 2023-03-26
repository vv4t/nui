#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#define LOG_DEBUG(fmt, ...) { fprintf(stdout, "[DEBUG] %s:%i:%s: ", __FILE__, __LINE__, __func__); fprintf(stdout, fmt, ##__VA_ARGS__); fprintf(stdout, "\n"); }
#define LOG_ERROR(fmt, ...) { fprintf(stderr, "[ERROR] %s:%i:%s: ", __FILE__, __LINE__, __func__); fprintf(stderr, fmt, ##__VA_ARGS__); fprintf(stderr, "\n"); }

#endif
