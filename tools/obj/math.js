"use strict";

export class vec2_t {
  constructor(x = 0, y = 0)
  {
    this.x = x;
    this.y = y;
  }
  
  add(v)
  {
    return new vec2_t(
      this.x + v.x,
      this.y + v.y
    );
  }
  
  sub(v)
  {
    return new vec2_t(
      this.x - v.x,
      this.y - v.y
    );
  }
  
  mul(v)
  {
    return new vec2_t(
      this.x * v.x,
      this.y * v.y
    );
  }
  
  mulf(f)
  {
    return new vec2_t(
      this.x * f,
      this.y * f
    );
  }
  
  dot(v)
  {
    return this.x * v.x + this.y * v.y;
  }
}

export class vec3_t {
  constructor(x = 0, y = 0, z = 0)
  {
    this.x = x;
    this.y = y;
    this.z = z;
  }
  
  dot(v)
  {
    return this.x * v.x + this.y * v.y + this.z * v.z;
  }
  
  add(v)
  {
    return new vec3_t(
      this.x + v.x,
      this.y + v.y,
      this.z + v.z
    );
  }
  
  sub(v)
  {
    return new vec3_t(
      this.x - v.x,
      this.y - v.y,
      this.z - v.z
    );
  }
  
  mulf(f)
  {
    return new vec3_t(
      this.x * f,
      this.y * f,
      this.z * f
    );
  }
  
  rotate(q)
  {
    return q.mul_vec3(this).mul(q.conjugate()).get_xyz();
  }
  
  length()
  {
    return Math.sqrt(this.dot(this));
  }
  
  normalize()
  {
    const length_squared = this.dot(this);
    
    if (length_squared == 0)
      return new vec3_t();
    
    const inverse_length = 1.0 / Math.sqrt(length_squared);
    
    return this.mulf(inverse_length);
  }
  
  toString()
  {
    return "vec3_t(" + this.x.toFixed(2) + ", " + this.y.toFixed(2) + ", " + this.z.toFixed(2) + ")";
  }
}

export class quat_t {
  constructor(x = 0, y = 0, z = 0, w = 1)
  {
    this.x = x;
    this.y = y;
    this.z = z;
    this.w = w;
  }
  
  mul(q)
  {
    return new quat_t(
      this.w * q.x + this.x * q.w + this.y * q.z - this.z * q.y,
      this.w * q.y + this.y * q.w + this.z * q.x - this.x * q.z,
      this.w * q.z + this.z * q.w + this.x * q.y - this.y * q.x,
      this.w * q.w - this.x * q.x - this.y * q.y - this.z * q.z
    );
  }
  
  mul_vec3(v)
  {
    return new quat_t(
      +this.w * v.x + this.y * v.z - this.z * v.y,
      +this.w * v.y + this.z * v.x - this.x * v.z,
      +this.w * v.z + this.x * v.y - this.y * v.x,
      -this.x * v.x - this.y * v.y - this.z * v.z
    );
  }
  
  conjugate()
  {
    return new quat_t(
      -this.x,
      -this.y,
      -this.z,
      +this.w
    );
  }
  
  get_xyz()
  {
    return new vec3_t(
      this.x,
      this.y,
      this.z
    );
  }
  
  static init_rotation(axis, angle)
  {
    const cos_rot = Math.cos(angle);
    const sin_rot = Math.sin(angle);
    
    return new quat_t(
      axis.x * sin_rot,
      axis.y * sin_rot,
      axis.z * sin_rot,
      cos_rot
    );
  }
}

export class plane_t {
  constructor(normal, distance)
  {
    this.normal = normal;
    this.distance = distance;
  }
  
  toString()
  {
    return "plane_t(" + this.normal + ", " + this.distance.toFixed(2) + ")";
  }
}

export class mat4_t {
  constructor()
  {
    this.m = new Float32Array(4 * 4);
  }
  
  mul(b)
  {
    const r = new mat4_t();
    
    for (let i = 0; i < 4; i++) {
      for (let j = 0; j < 4; j++) {
        r.m[i * 4 + j] = b.m[0 * 4 + j] * this.m[i * 4 + 0] +
                         b.m[1 * 4 + j] * this.m[i * 4 + 1] +
                         b.m[2 * 4 + j] * this.m[i * 4 + 2] +
                         b.m[3 * 4 + j] * this.m[i * 4 + 3];
                      
      }
    }
    
    return r;
  }
  
  static init_identity()
  {
    const m = new mat4_t();
    
    m.m[0] = 1; m.m[4]  = 0; m.m[8]  = 0; m.m[12] = 0;
    m.m[1] = 0; m.m[5]  = 1; m.m[9]  = 0; m.m[13] = 0;
    m.m[2] = 0; m.m[6]  = 0; m.m[10] = 1; m.m[14] = 0;
    m.m[3] = 0; m.m[7]  = 0; m.m[11] = 0; m.m[15] = 1;
    
    return m;
  }
  
  static init_translation(v)
  {
    const m = new mat4_t();
    
    m.m[0] = 1; m.m[4]  = 0; m.m[8]  = 0; m.m[12] = v.x;
    m.m[1] = 0; m.m[5]  = 1; m.m[9]  = 0; m.m[13] = v.y;
    m.m[2] = 0; m.m[6]  = 0; m.m[10] = 1; m.m[14] = v.z;
    m.m[3] = 0; m.m[7]  = 0; m.m[11] = 0; m.m[15] = 1;
    
    return m;
  }
  
  static init_rotation(q)
  {
    const m = new mat4_t();
    
    const x = q.x;
    const y = q.y;
    const z = q.z;
    const w = q.w;
    
    m.m[0] = 1-2*y*y - 2*z*z;   m.m[4] = 2*x*y - 2*z*w;     m.m[8]  = 2*x*z + 2*y*w;    m.m[12] = 0;
    m.m[1] = 2*x*y + 2*z*w;     m.m[5] = 1-2*x*x - 2*z*z;   m.m[9]  = 2*y*z - 2*x*w;    m.m[13] = 0;
    m.m[2] = 2*x*z - 2*y*w;     m.m[6] = 2*y*z + 2*x*w;     m.m[10] = 1-2*x*x - 2 *y*y; m.m[14] = 0;
    m.m[3] = 0;                 m.m[7] = 0;                 m.m[11] = 0;                m.m[15] = 1;
    
    return m;
  }
  
  static init_orthogonal(l, r, t, b, n, f)
  {
    const m = new mat4_t();
    
    m.m[0] = 2 / (r - l);   m.m[4] = 0;             m.m[8]  = 0;            m.m[12] = -(r + l) / (r - l);
    m.m[1] = 0;             m.m[5] = 2 / (t - b);   m.m[9]  = 0;            m.m[13] = -(t + b) / (t - b);
    m.m[2] = 0;             m.m[6] = 0;             m.m[10] = -2 / (f - n); m.m[14] = -(f + n) / (f - n);
    m.m[3] = 0;             m.m[7] = 0;             m.m[11] = 0;            m.m[15] = 1;
    
    return m;
  }
  
  static init_perspective(aspect_ratio, fov, near, far)
  {
    const m = new mat4_t();
    
    const tan_fov = 1 / Math.tan(fov / 2);
    const ar_tan_fov = aspect_ratio * tan_fov;
    
    const z_scale = (-near-far) / (near-far);
    const z_offset = (2 * far * near) / (near - far);
    
    m.m[0]  = ar_tan_fov; m.m[4]  = 0;        m.m[8]  = 0;        m.m[12] = 0;
    m.m[1]  = 0;          m.m[5]  = tan_fov;  m.m[9]  = 0;        m.m[13] = 0;
    m.m[2]  = 0;          m.m[6]  = 0;        m.m[10] = z_scale;  m.m[14] = z_offset;
    m.m[3]  = 0;          m.m[7]  = 0;        m.m[11] = 1;        m.m[15] = 0;
    
    return m;
  }
}
