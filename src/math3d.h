#ifndef MATH3D_H
#define MATH3D_H

#include <ostream>
#include <cmath>

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

class mat4 {
private:
  float m[16];

public:
  inline mat4(vec4 a, vec4 b, vec4 c, vec4 d) {
    m[ 0] = a.x;  m[ 1] = a.y;  m[ 2] = a.z;  m[ 3] = a.w;
    m[ 4] = b.x;  m[ 5] = b.y;  m[ 6] = b.z;  m[ 7] = b.w;
    m[ 8] = c.x;  m[ 9] = c.y;  m[10] = c.z;  m[11] = c.w;
    m[12] = d.x;  m[13] = d.y;  m[14] = d.z;  m[15] = d.w;
  }
  
  inline mat4() {
    *this = mat4::identity();
  }
  
  static mat4 translate(vec3 v) {
    return mat4(
      vec4(1, 0, 0, 0),
      vec4(0, 1, 0, 0),
      vec4(0, 0, 1, 0),
      vec4(v.x, v.y, v.z, 1)
    );
  }
  
  static mat4 perspective() {
    return mat4(
      vec4(1, 0, 0, 0),
      vec4(0, 1, 0, 0),
      vec4(0, 0, 1, 1),
      vec4(0, 0, 0, 0)
    );
  }
  
  static mat4 identity() {
    return mat4(
      vec4(1, 0, 0, 0),
      vec4(0, 1, 0, 0),
      vec4(0, 0, 1, 0),
      vec4(0, 0, 0, 1)
    );
  }
  
  static mat4 rotate_x(float t) {
    return mat4(
      vec4(1, 0, 0, 0),
      vec4(0, +cos(t), sin(t), 0),
      vec4(0, -sin(t), cos(t), 0),
      vec4(0, 0, 0, 1)
    );
  }
  
  static mat4 rotate_y(float t) {
    return mat4(
      vec4(+cos(t), 0, sin(t), 0),
      vec4(0, 1, 0, 0),
      vec4(-sin(t), 0, cos(t), 0),
      vec4(0, 0, 0, 1)
    );
  }
  
  static mat4 rotate_z(float t) {
    return mat4(
      vec4(+cos(t), sin(t), 0, 0),
      vec4(-sin(t), cos(t), 0, 0),
      vec4(0, 0, 1, 0),
      vec4(0, 0, 0, 1)
    );
  }
  
  inline friend mat4 operator*(const mat4& a, const mat4& b) {
    mat4 r;
    
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        r.m[i * 4 + j] = 0.0;
        
        for (int k = 0; k < 4; k++) {
          r.m[i * 4 + j] += a.m[i * 4 + k] * b.m[k * 4 + j];
        }
      }
    }
    
    return r;
  }
  
  inline friend std::ostream& operator<<(std::ostream& stream, const mat4& m) {
    stream << "mat4 {" << std::endl;
    stream << "  " << m.m[ 0] << " " << m.m[ 4] << " " << m.m[ 8] << " " << m.m[12] << std::endl;
    stream << "  " << m.m[ 1] << " " << m.m[ 5] << " " << m.m[ 9] << " " << m.m[13] << std::endl;
    stream << "  " << m.m[ 2] << " " << m.m[ 6] << " " << m.m[10] << " " << m.m[14] << std::endl;
    stream << "  " << m.m[ 3] << " " << m.m[ 7] << " " << m.m[11] << " " << m.m[15] << std::endl;
    stream << "}";
    return stream;
  }
};

#endif
