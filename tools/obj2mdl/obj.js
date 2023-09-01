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

class material_t {
  constructor(name)
  {
    this.name = name;
    this.diffuse = "";
  }
};

class object_t {
  constructor(material, faces)
  {
    this.material = material;
    this.faces = faces;
  }
};

class mtllib_t {
  constructor()
  {
    this.materials = [];
  }
  
  load(str_path)
  {
    const dir = path.parse(str_path).dir;
    
    const file = fs.readFileSync(str_path).toString();
    const lines = file.split('\n');
    
    let material = null;
    
    this.materials = [];
    
    for (const line of lines) {
      const args = line.split(' ').filter((x) => x.length > 0);
      
      if (args[0] == "newmtl") {
        if (material) {
          this.materials.push(material);
        }
        
        material = new material_t(args[1]);
      } else if (args[0] == "map_Kd") {
        material.diffuse = args[1];
      }
    }
    
    if (material) {
      this.materials.push(material);
    }
  }
  
  find(name)
  {
    for (let i = 0; i < this.materials.length; i++) {
      if (name == this.materials[i].name) {
        return i;
      }
    }
    
    return -1;
  }
};

class obj_t {
  constructor(materials, objects)
  {
    this.materials = materials;
    this.objects = objects;
  }
};

class vertex_group_t {
  constructor(material, offset, count)
  {
    this.material = material;
    this.offset = offset;
    this.count = count;
  }
};

class mdl_t {
  constructor(vertex_groups, vertices)
  {
    this.vertex_groups = vertex_groups;
    this.vertices = vertices;
  }
};

function obj_to_mdl(obj)
{
  const object_groups = {};
  
  for (const object of obj.objects) {
    if (!(object.material in object_groups)) {
      object_groups[object.material] = [];
    }
    
    object_groups[object.material].push(object);
  }
  
  const vertices = [];
  const vertex_groups = [];
  
  for (const [material, objects] of Object.entries(object_groups)) {
    const offset = vertices.length;
    
    for (const object of objects) {
      for (const face of object.faces) {
        vertices.push(face.v1);
        vertices.push(face.v2);
        vertices.push(face.v3);
      }
    }
    
    const count = vertices.length - offset;
    
    vertex_groups.push(new vertex_group_t(obj.materials[material], offset, count));
  }
  
  return new mdl_t(vertex_groups, vertices);
}

function main()
{
  if (process.argv.length != 4) {
    console.log("usage:", path.parse(process.argv[1]).name, "[obj-file] [mdl-file]");
    process.exit(1);
  }
  
  const input_obj = process.argv[2];
  const output_mdl = process.argv[3];
  
  const obj = obj_parse(input_obj);
  const mdl = obj_to_mdl(obj);
  
  const write = new write_t();
  
  write.write_u32(mdl.vertex_groups.length);
  write.write_u32(mdl.vertices.length);
  
  for (const vertex_group of mdl.vertex_groups) {
    write.write_s32(vertex_group.material.diffuse);
    
    write.write_u32(vertex_group.offset);
    write.write_u32(vertex_group.count);
  }
  
  for (const vertex of mdl.vertices) {
    write.write_vertex(vertex);
  }
  
  fs.writeFileSync(output_mdl, Buffer.from(write.data()));
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
  let objects = [];
  let materials = [];
  
  let material = 0;
  let object = null;
  
  let mtllib = new mtllib_t();
  
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
    } else if (args[0] == "usemtl") {
      material = mtllib.find(args[1]);
    } else if (args[0] == "mtllib") {
      mtllib.load(path.join(dir, args[1]));
      materials.push(...mtllib.materials);
    } else if (args[0] == "o") {
      if (face_buf.length > 0) {
        objects.push(new object_t(material, face_buf));
        face_buf = [];
      }
    }
  }
  
  if (face_buf.length > 0) {
    objects.push(new object_t(material, face_buf));
  }
  
  return new obj_t(materials, objects);
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
  
  write_s32(s32)
  {
    for (let i = 0; i < Math.min(s32.length, 32); i++) {
      this.write_u8(s32.charCodeAt(i));
    }
    
    for (let i = s32.length; i < 32; i++) {
      this.write_u8(0);
    }
  }
};

function align(x, align)
{
  return Math.floor(x / align);
}

main();
