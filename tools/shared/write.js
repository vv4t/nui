
class sizeof {
  static U8       = 1;
  static U16      = 2;
  static U32      = 4;
  static F32      = 4;
  
  static VEC2_T   = 2 * sizeof.F32;
  static VEC3_T   = 3 * sizeof.F32;
  
  static VERTEX_T = 2 * sizeof.VEC3_T + 1 * sizeof.VEC3_t;
};

export class write_t {
  constructor()
  {
    this.b = new Uint8Array(8);
    this.pos = 0;
    this.end_pos = this.pos;
  }
  
  data()
  {
    const new_b = new Uint8Array(this.pos);
    
    for (let i = 0; i < this.pos; i++) {
      new_b [i] = this.b[i];
    }
    
    return new_b;
  }
  
  grow()
  {
    const new_b = new Uint8Array(2 * this.b.byteLength);
    new_b.set(this.b); 
    this.b = new_b;
  }
  
  seek(pos)
  {
    this.pos = pos;
    
    if (this.pos > this.end_pos) {
      this.end_pos = this.pos;
    }
    
    while (pos >= this.b.byteLength) {
      this.grow();
    }
  }
  
  tell()
  {
    return this.pos;
  }
  
  write_u8(u8)
  {
    this.b[this.pos] = u8|0;
    this.seek(this.pos + sizeof.U8);
  }
  
  write_u16(u16)
  {
    this.write_u8((u16) & 0xff);
    this.write_u8((u16 >> 8) & 0xff);
  }
  
  write_u32(u32)
  {
    this.write_u8((u32) & 0xff);
    this.write_u8((u32 >> 8) & 0xff);
    this.write_u8((u32 >> 16) & 0xff);
    this.write_u8((u32 >> 24) & 0xff);
  }
  
  write_f32(f32)
  {
    const b_f32 = new Float32Array(1);
    b_f32[0] = f32;
    
    const b_u8 = new Uint8Array(b_f32.buffer);
    
    this.write_u8(b_u8[0]);
    this.write_u8(b_u8[1]);
    this.write_u8(b_u8[2]);
    this.write_u8(b_u8[3]);
  }
  
  write_vec2(vec2)
  {
    this.write_f32(vec2.x);
    this.write_f32(vec2.y);
  }
  
  write_vec3(vec3)
  {
    this.write_f32(vec3.x);
    this.write_f32(vec3.y);
    this.write_f32(vec3.z);
  }
  
  write_vertex(vertex)
  {
    this.write_vec3(vertex.pos);
    this.write_vec3(vertex.normal);
    this.write_vec2(vertex.uv);
  }
  
  write_s32(s32)
  {
    for (let i = 0; i < Math.min(s32.length, 64); i++) {
      this.write_u8(s32.charCodeAt(i));
    }
    
    for (let i = s32.length; i < 64; i++) {
      this.write_u8(0);
    }
  }
};
