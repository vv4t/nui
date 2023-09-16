"use strict";

import path from "path";
import fs from "fs";
import { vec2_t, vec3_t } from "./math.js";

export function obj_parse(str_path)
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

