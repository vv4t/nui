#ifndef MATH3D_H
#define MATH3D_H

class vec2 {
public:
  float x, y;
  
  inline vec2() {
    x = 0.0f;
    y = 0.0f;
  }
  
  inline vec2(float x_, float y_) {
    x = x_;
    y = y_;
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
  
  inline vec3(float x_, float y_, float z_) {
    x = x_;
    y = y_;
    z = z_;
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
  
  inline vec4(float x_, float y_, float z_, float w_) {
    x = x_;
    y = y_;
    z = z_;
    w = w_;
  }
};

#endif
