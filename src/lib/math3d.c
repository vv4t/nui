#include <math3d.h>

#include <stdio.h>

void vector_print(const char *string, vector x)
{
  printf("%s: <%.6f, %.6f, %.6f, %.6f>\n", string, x.x, x.y, x.z, x.w); 
}

void matrix_print(const char *string, matrix A)
{
  printf("%s: [\n", string);
  printf("  <%.6f, %.6f, %.6f, %.6f>\n", A.m[ 0], A.m[ 4], A.m[ 8], A.m[12]); 
  printf("  <%.6f, %.6f, %.6f, %.6f>\n", A.m[ 1], A.m[ 5], A.m[ 9], A.m[13]); 
  printf("  <%.6f, %.6f, %.6f, %.6f>\n", A.m[ 2], A.m[ 6], A.m[10], A.m[14]); 
  printf("  <%.6f, %.6f, %.6f, %.6f>\n", A.m[ 3], A.m[ 7], A.m[11], A.m[15]);
  printf("]\n");
}
