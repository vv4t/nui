#ifndef MATH3D_H
#define MATH3D_H

class vec2 {
public:
  float x, y;
  
  inline vec2() {
    x = 0.0f;
    y = 0.0f;
  }
  
  inline vec2(float _x, float _y) {
    x = _x;
    y = _y;
  }
};

class vec3 {
public:
  float x, y, z;
  
  inline vec3() {
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
  }
  
  inline vec3(float _x, float _y, float _z) {
    x = _x;
    y = _y;
    z = _z;
  }
};

class vec4 {
public:
  float x, y, z, w;
  
  inline vec4() {
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
    w = 1.0f;
  }
  
  inline vec4(float _x, float _y, float _z, float _w) {
    x = _x;
    y = _y;
    z = _z;
    w = _w;
  }
};

#endif
