#ifndef MATH3D_H
#define MATH3D_H

#include <ostream>
#include <cmath>

class vec2 {
public:
  float x, y;
  
  
  inline vec2(float x_, float y_) {
    x = x_;
    y = y_;
  }
  
  inline vec2(float f) : vec2(f, f) {}
  inline vec2() : vec2(0.0) {}
  
  inline vec2 operator-() {
    return vec2(-x, -y);
  }
  
  inline friend vec2 operator+(const vec2& a, const vec2& b) {
    return vec2(a.x + b.x, a.y + b.y);
  }
  
  inline friend vec2 operator-(const vec2& a, const vec2& b) {
    return vec2(a.x - b.x, a.y - b.y);
  }
  
  inline friend vec2 operator+(const vec2& a, float b) {
    return a + vec2(b, b);
  }
  
  inline friend vec2 operator-(const vec2& a, float b) {
    return a - vec2(b, b);
  }
  
  inline friend vec2 operator*(const vec2& a, float b) {
    return vec2(a.x * b, a.y * b);
  }
  
  inline friend std::ostream& operator<<(std::ostream& stream, const vec2& m) {
    stream << "vec2(" << m.x << ", " << m.y << ")";
    return stream;
  }
};

class vec3 {
public:
  float x, y, z;
  
  inline vec3(float x_, float y_, float z_) {
      x = x_;
    y = y_;
    z = z_;
  }
  
  inline vec3(vec2 xy, float z_) : vec3(xy.x, xy.y, z_) {}
  inline vec3(float f) : vec3(f, f, f) {}
  inline vec3() : vec3(0.0, 0.0, 0.0) {}
  inline vec2 get_xy() { return vec2(x, y); }
  
  inline bool is_positive() {
    return x >= 0 && y >= 0 && z >= 0;
  }
  
  inline bool is_negative() {
    return !is_positive();
  }
  
  inline vec3 reduce_to_min_axis() {
    if (x < y) {
      return x < z ? vec3(x, 0, 0) : vec3(0, 0, z);
    } else {
      return y < z ? vec3(0, y, 0) : vec3(0, 0, z);
    }
  }
  
  inline float length() {
    return sqrt(length_squared());
  }
  
  inline float length_squared() {
    return dot(*this, *this);
  }
  
  inline vec3 normalize() {
    return *this * (1.0 / length());
  }
  
  inline static float dot(vec3 a, vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
  }
  
  inline static vec3 min(vec3 a, vec3 b) {
    return vec3(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
  }
  
  inline static vec3 max(vec3 a, vec3 b) {
    return vec3(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
  }
  
  inline vec3 operator-() {
    return vec3(-x, -y, -z);
  }
  
  inline friend vec3 operator-(const vec3& v) {
    return vec3(-v.x, -v.y, -v.z);
  }
  
  inline friend vec3 operator+(const vec3& v) {
    return v;
  }
  
  inline friend vec3 operator+(const vec3& a, const vec3& b) {
    return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
  }
  
  inline friend vec3 operator-(const vec3& a, const vec3& b) {
    return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
  }
  
  inline friend vec3 operator*(const vec3& a, float b) {
    return vec3(a.x * b, a.y * b, a.z * b);
  }
  
  inline friend vec3& operator*=(vec3& a, float b) {
    return a = a * b;
  }
  
  inline friend vec3& operator+=(vec3& a, const vec3& b) {
    return a = a + b;
  }
  
  inline friend std::ostream& operator<<(std::ostream& stream, const vec3& m) {
    stream << "vec3(" << m.x << ", " << m.y << ", " << m.z << ")";
    return stream;
  }
};

class vec4 {
public:
  float x, y, z, w;
  
  inline vec4(float x_, float y_, float z_, float w_) {
    x = x_;
    y = y_;
    z = z_;
    w = w_;
  }
  
  inline vec4(vec3 xyz, float w_) : vec4(xyz.x, xyz.y, xyz.z, w_) {}
  inline vec4(vec2 xy, float z_, float w_) : vec4(xy.x, xy.y, z_, w_) {}
  inline vec4() : vec4(0.0, 0.0, 0.0, 1.0) {}
  inline vec2 get_xy() { return vec2(x, y); }
  inline vec3 get_xyz() { return vec3(x, y, z); }
  
  inline vec3 operator-() {
    return vec3(-x, -y, -z);
  }
  
  inline friend vec4 operator+(const vec4& a, const vec4& b) {
    return vec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
  }
  
  inline friend vec4 operator-(const vec4& a, const vec4& b) {
    return vec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
  }
  
  inline friend vec4 operator*(const vec4& a, float b) {
    return vec4(a.x * b, a.y * b, a.z * b, a.w * b);
  }
  
  inline friend std::ostream& operator<<(std::ostream& stream, const vec4& m) {
    stream << "vec3(" << m.x << ", " << m.y << ", " << m.z << ", " << m.w << ")";
    return stream;
  }
};

class mat2 {
private:
  float m[4];

public:
  inline mat2(vec2 a, vec2 b) {
    m[0] = a.x;  m[1] = a.y;
    m[2] = b.x;  m[3] = b.y;
  }
  
  inline mat2() {
    *this = mat2::identity();
  }
  
  inline static mat2 identity() {
    return mat2(
      vec2(1, 0),
      vec2(0, 1)
    );
  }
  
  inline friend vec2 operator*(const vec2& a, const mat2& b) {
    return vec2(b.m[0] * a.x + b.m[1] * a.x, b.m[2] * a.x + b.m[3] * a.y);
  }
  
  inline friend mat2 operator*(const mat2& a, const mat2& b) {
    mat2 r;
    
    for (int i = 0; i < 2; i++) {
      for (int j = 0; j < 2; j++) {
        r.m[i * 2 + j] = 0.0;
        
        for (int k = 0; k < 4; k++) {
          r.m[i * 2 + j] += a.m[i * 2 + k] * b.m[k * 2 + j];
        }
      }
    }
    
    return r;
  }
  
  inline friend std::ostream& operator<<(std::ostream& stream, const mat2& m) {
    stream << "mat2 {" << std::endl;
    stream << "  " << m.m[0] << " " << m.m[2] << std::endl;
    stream << "  " << m.m[1] << " " << m.m[3] << std::endl;
    stream << "}";
    return stream;
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
  
  inline mat4(vec3 a, vec3 b, vec3 c) :
    mat4(
      vec4(a.x, a.y, a.z, 0),
      vec4(b.x, b.y, b.z, 0),
      vec4(c.x, c.y, c.z, 0),
      vec4(0, 0, 0, 1)
    ) {}
  
  inline mat4() {
    *this = mat4::identity();
  }
  
  inline static mat4 translate(vec3 v) {
    return mat4(
      vec4(1, 0, 0, 0),
      vec4(0, 1, 0, 0),
      vec4(0, 0, 1, 0),
      vec4(v.x, v.y, v.z, 1)
    );
  }
  
  inline static mat4 scale(vec3 v) {
    return mat4(
      vec4(v.x, 0, 0, 0),
      vec4(0, v.y, 0, 0),
      vec4(0, 0, v.z, 0),
      vec4(0, 0, 0, 1)
    );
  }
  
  static mat4 perspective(float aspect_ratio = 1.0, float fov = M_PI / 2.0, float near = 0.01, float far = 100.0) {
    float tan_fov = 1 / tan(fov / 2);
    float ar_tan_fov = aspect_ratio * tan_fov;  

    float z_scale = (-far + -near) / (-far - -near);
    float z_offset = (2 * -far * -near) / (-far - -near);
    
    return mat4(
      vec4(ar_tan_fov, 0, 0, 0),
      vec4(0, tan_fov, 0, 0),
      vec4(0, 0, z_scale, 1),
      vec4(0, 0, z_offset, 0)
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
  
  static mat4 rotate_zyx(vec3 rotation) {
    return mat4::rotate_z(rotation.z) * mat4::rotate_y(rotation.y) * mat4::rotate_x(rotation.x);
  }
  
  static mat4 rotate_yzx(vec3 rotation) {
    return mat4::rotate_y(rotation.y) * mat4::rotate_z(rotation.z) * mat4::rotate_x(rotation.x);
  }
  
  static mat4 rotate_yxz(vec3 rotation) {
    return mat4::rotate_y(rotation.y) * mat4::rotate_x(rotation.x) * mat4::rotate_z(rotation.z);
  }
  
  static mat4 rotate_xyz(vec3 rotation) {
    return mat4::rotate_x(rotation.x) * mat4::rotate_y(rotation.y) * mat4::rotate_z(rotation.z);
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
  
  inline friend vec4 operator*(const mat4& m, const vec4& v) {
    return vec4(
      m.m[ 0] * v.x + m.m[ 4] * v.y + m.m[ 8] * v.z + m.m[12] * v.w,
      m.m[ 1] * v.x + m.m[ 5] * v.y + m.m[ 9] * v.z + m.m[13] * v.w,
      m.m[ 2] * v.x + m.m[ 6] * v.y + m.m[10] * v.z + m.m[14] * v.w,
      m.m[ 3] * v.x + m.m[ 7] * v.y + m.m[11] * v.z + m.m[15] * v.w
    );
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
