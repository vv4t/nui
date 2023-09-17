"use strict";

import fs from "fs";
import path from "path";
import { write_t } from "../shared/write.js";
import { obj_parse } from "../shared/obj.js";

class vertex_group_t {
  constructor(material, offset, count)
  {
    this.material = material;
    this.offset = offset;
    this.count = count;
  }
};

class mdl_vertex_t {
  constructor(pos, normal, uv)
  {
    this.pos = pos;
    this.normal = normal;
    this.uv = uv;
  }
};

class mdl_t {
  constructor(vertex_groups, vertices)
  {
    this.vertex_groups = vertex_groups;
    this.vertices = vertices;
  }
};

function main()
{
  if (process.argv.length != 4) {
    console.log("usage:", path.parse(process.argv[1]).name, "[obj-file] [mdl-file]");
    process.exit(1);
  }
  
  const input_obj = process.argv[2];
  const out_mdl = process.argv[3];
  
  const obj = obj_parse(input_obj);
  const mdl = obj_to_mdl(obj);
  
  const write = new write_t();
  
  write_mdl(write, mdl, out_mdl);
}

function write_mdl(write, mdl, out_mdl)
{
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
  
  fs.writeFileSync(out_mdl, Buffer.from(write.data()));
}

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
        vertices.push(new mdl_vertex_t(face.vertices[0].pos, face.normal, face.vertices[0].uv));
        vertices.push(new mdl_vertex_t(face.vertices[1].pos, face.normal, face.vertices[1].uv));
        vertices.push(new mdl_vertex_t(face.vertices[2].pos, face.normal, face.vertices[2].uv));
      }
    }
    
    const count = vertices.length - offset;
    
    vertex_groups.push(new vertex_group_t(obj.materials[material], offset, count));
  }
  
  return new mdl_t(vertex_groups, vertices);
}

main();
