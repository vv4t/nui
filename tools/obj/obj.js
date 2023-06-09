"use strict";

import fs from "fs";
import path from "path";
import { vec2_t, vec3_t } from "./math.js";

class sizeof {
  static U8       = 1;
  static U16      = 2;
  static U32      = 4;
  static F32      = 4;
  
  static VEC2_T   = 2 * sizeof.F32;
  static VEC3_T   = 3 * sizeof.F32;
  
  static VERTEX_T = 2 * sizeof.VEC3_T + 1 * sizeof.VEC3_t;
};

class vertex_t {
  constructor(pos, normal, uv)
  {
    this.pos = pos;
    this.normal = normal;
    this.uv = uv;
  }
};

class face_t {
  constructor(v1, v2, v3)
  {
    this.v1 = v1;
    this.v2 = v2;
    this.v3 = v3;
  }
};

function main()
{
  const face_buf = obj_parse("saves/scene.obj");
  
  const write = new write_t();
  
  write.write_u32(face_buf.length);
  
  for (const face of face_buf) {
    write.write_vertex(face.v1);
    write.write_vertex(face.v2);
    write.write_vertex(face.v3);
  }
  
  fs.writeFileSync("../../res/mesh/scene.mesh", Buffer.from(write.data()));
}

function obj_parse(str_path)
{
  const dir = path.parse(str_path).dir;
  
  const file = fs.readFileSync(str_path).toString();
  const lines = file.split('\n');
  
  let pos_buf = [];
  let normal_buf = [];
  let uv_buf = [];
  
  let face_buf = [];
  
  for (const line of lines) {
    const args = line.split(' ').filter((x) => x.length > 0);
    
    if (args[0] == "v") {
      pos_buf.push(
        new vec3_t(
          parseFloat(args[1]),
          parseFloat(args[2]),
          parseFloat(args[3])
        )
      );
    } else if (args[0] == "vt") {
      uv_buf.push(
        new vec2_t(
          parseFloat(args[1]),
          parseFloat(args[2])
        )
      );
    } else if (args[0] == "vn") {
      normal_buf.push(
        new vec3_t(
          parseFloat(args[1]),
          parseFloat(args[2]),
          parseFloat(args[3])
        )
      );
    } else if (args[0] == "f") {
      const vertices = [];
      
      for (let i = 1; i < 4; i++) {
        const face_data = args[i].split('/').map((x) => parseInt(x, 10));
        
        const pos_id = face_data[0] - 1;
        const normal_id = face_data[2] - 1;
        const uv_id = face_data[1] - 1;
        
        const vertex = new vertex_t(
          pos_buf[pos_id],
          normal_buf[normal_id],
          uv_buf[uv_id]
        );
        
        vertices.push(vertex);
      }
      
      const face = new face_t(vertices[0], vertices[1], vertices[2]);
      
      face_buf.push(face);
    }
  }
  
  return face_buf;
}

class write_t {
  constructor()
  {
    this.b = new ArrayBuffer(8);
    this._reset();
    
    this.pos = 0;
    this.max_pos = this.pos;
  }
  
  data()
  {
    const new_b = new ArrayBuffer(this.max_pos + 1);
    const new_b_u8 = new Uint8Array(new_b);
    
    for (let i = 0; i < this.max_pos; i++)
      new_b_u8[i] = this.b_u8[i];
    
    return new_b;
  }
  
  _reset()
  {
    this.b_u8 = new Uint8Array(this.b);
    this.b_u16 = new Uint16Array(this.b);
    this.b_u32 = new Uint32Array(this.b);
    this.b_f32 = new Float32Array(this.b);
  }
  
  grow()
  {
    const new_b = new ArrayBuffer(2 * this.b.byteLength);
    new Uint8Array(new_b).set(this.b_u8);
    this.b = new_b;
    this._reset();
  }
  
  seek(pos)
  {
    this.pos = pos;
    
    if (this.pos > this.max_pos)
      this.max_pos = this.pos;
    
    while (pos >= this.b.byteLength)
      this.grow();
  }
  
  tell()
  {
    return this.pos;
  }
  
  write_u8(u8)
  {
    this.b_u8[align(this.pos, sizeof.U8)] = u8|0;
    this.seek(this.pos + sizeof.U8);
  }
  
  write_u16(u16)
  {
    this.b_u16[align(this.pos, sizeof.U16)] = u16|0;
    this.seek(this.pos + sizeof.U16);
  }
  
  write_u32(u32)
  {
    this.b_u32[align(this.pos, sizeof.U32)] = u32|0;
    this.seek(this.pos + sizeof.U32);
  }
  
  write_f32(f32)
  {
    this.b_f32[align(this.pos, sizeof.F32)] = f32;
    this.seek(this.pos + sizeof.F32);
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
};

function align(x, align)
{
  return Math.floor(x / align);
}

main();
