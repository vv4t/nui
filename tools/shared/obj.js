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
  let name = "";
  
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
      let normal;
      
      for (let i = 1; i < 4; i++) {
        const face_data = args[i].split('/').map((x) => parseInt(x, 10));
        
        const pos_id = face_data[0] - 1;
        const normal_id = face_data[2] - 1;
        const uv_id = face_data[1] - 1;
        
        const vertex = new obj_vertex_t(
          pos_buf[pos_id],
          uv_buf[uv_id]
        );
        
        normal = normal_buf[normal_id];
        vertices.push(vertex);
      }
      
      const face = new obj_face_t([vertices[0], vertices[1], vertices[2]], normal);
      
      face_buf.push(face);
    } else if (args[0] == "usemtl") {
      material = mtllib.find(args[1]);
    } else if (args[0] == "mtllib") {
      mtllib.load(path.join(dir, args[1]));
      materials.push(...mtllib.materials);
    } else if (args[0] == "o") {
      if (face_buf.length > 0) {
        objects.push(new obj_object_t(name, material, face_buf));
        face_buf = [];
      }
      
      name = args[1];
    }
  }
  
  if (face_buf.length > 0) {
    objects.push(new obj_object_t(name, material, face_buf));
  }
  
  return new obj_t(materials, objects);
}

export class obj_vertex_t {
  constructor(pos, uv)
  {
    this.pos = pos;
    this.uv = uv;
  }
};

export class obj_face_t {
  constructor(vertices, normal)
  {
    this.vertices = vertices;
    this.normal = normal;
  }
};

export class obj_material_t {
  constructor(name)
  {
    this.name = name;
    this.diffuse = "";
    this.normal = "";
    this.color = new vec3_t(1.0, 1.0, 1.0);
    this.specular = 0.0;
    this.alpha = 1.0;
  }
};

export class obj_object_t {
  constructor(name, material, faces)
  {
    this.name = name;
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
        
        material = new obj_material_t(args[1]);
      } else if (args[0] == "map_Kd") {
        material.diffuse = args[1];
      } else if (args[0] == "map_Bump") {
        material.normal = args[3];
      } else if (args[0] == "Kd") {
        material.color = new vec3_t(parseFloat(args[1]), parseFloat(args[2]), parseFloat(args[3]));
      } else if (args[0] == "Ns") {
        material.specular = parseFloat(args[1]);
      } else if (args[0] == "d") {
        material.alpha = parseFloat(args[1]);
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

