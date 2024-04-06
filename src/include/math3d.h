#ifndef MATH3D_H
#define MATH3D_H

#define _USE_MATH_DEFINES

#include <math.h>

typedef struct {
  float x;
  float y;
  float z;
  float w;
} vector;

typedef struct {
  float m[16];
} matrix;

void vector_print(const char *string, vector x);
void matrix_print(const char *string, matrix A);

inline static vector vec2(float x, float y) { return (vector) { .x = x, .y = y, .z = 0, .w = 0 }; }
inline static vector vec3(float x, float y, float z) { return (vector) { .x = x, .y = y, .z = z, .w = 0 }; }
inline static vector vec4(float x, float y, float z, float w) { return (vector) { .x = x, .y = y, .z = z, .w = w }; }

inline static vector v2pt(vector x) { return (vector) { .x = x.x, .y = x.y, .z = x.z, .w = 1.0 }; }

inline static matrix mat4(vector a, vector b, vector c, vector d)
{
  return (matrix) {
    .m[ 0] = a.x, .m[ 4] = a.y, .m[ 8] = a.z, .m[12] = a.w,
    .m[ 1] = b.x, .m[ 5] = b.y, .m[ 9] = b.z, .m[13] = b.w,
    .m[ 2] = c.x, .m[ 6] = c.y, .m[10] = c.z, .m[14] = c.w,
    .m[ 3] = d.x, .m[ 7] = d.y, .m[11] = d.z, .m[15] = d.w
  };
}

inline static vector vaddv(vector A, vector B)
{
  return (vector) {
    .x = A.x + B.x,
    .y = A.y + B.y,
    .z = A.z + B.z,
    .w = A.w + B.w
  };
}

inline static vector fdotv(float a, vector x)
{
  return (vector) {
    .x = a * x.x,
    .y = a * x.y,
    .z = a * x.z,
    .w = a * x.w
  };
}

inline static vector mdotv(matrix A, vector x)
{
  return vec4(
    A.m[ 0] * x.x + A.m[ 4] * x.y + A.m[ 8] * x.z + A.m[12] * x.w,
    A.m[ 1] * x.x + A.m[ 5] * x.y + A.m[ 9] * x.z + A.m[13] * x.w,
    A.m[ 2] * x.x + A.m[ 6] * x.y + A.m[10] * x.z + A.m[14] * x.w,
    A.m[ 3] * x.x + A.m[ 7] * x.y + A.m[11] * x.z + A.m[15] * x.w
  );
}

inline static matrix mdotm(matrix A, matrix B)
{
  matrix R;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      R.m[i * 4 + j] = 0.0;
      
      for (int k = 0; k < 4; k++) {
        R.m[i * 4 + j] += A.m[i * 4 + k] * B.m[k * 4 + j];
      }
    }
  }
  
  return R;
}

inline static matrix identity()
{
  return mat4(
    vec4(1, 0, 0, 0),
    vec4(0, 1, 0, 0),
    vec4(0, 0, 1, 0),
    vec4(0, 0, 0, 1)
  );
}

inline static matrix translate(vector x)
{
  return mat4(
    vec4(1, 0, 0, x.x),
    vec4(0, 1, 0, x.y),
    vec4(0, 0, 1, x.z),
    vec4(0, 0, 0, 1)
  );
}

inline static matrix scale(vector x)
{
  return mat4(
    vec4(x.x, 0, 0, 0),
    vec4(0, x.y, 0, 0),
    vec4(0, 0, x.z, 0),
    vec4(0, 0, 0,   1)
  );
}

inline static matrix rotate_z(float t)
{
  float c = cos(t);
  float s = sin(t);
  
  return mat4(
    vec4(c,   -s,   0,    0),
    vec4(s,   +c,   0,    0),
    vec4(0,   0,    1,    0),
    vec4(0,   0,    0,    1)
  );
}

inline static matrix transform(vector t, vector r, vector s)
{
  return mdotm(scale(s), mdotm(rotate_z(r.z), translate(t)));
}

#endif
