"use strict";

import fs from "fs";
import path from "path";
import { write_t } from "../shared/write.js";
import { obj_parse } from "../shared/obj.js";

class mdl_subgroup {
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

class mdl_material_t {
  constructor(diffuse, normal, color, specular)
  {
    this.diffuse = diffuse;
    this.normal = normal;
    this.color = color;
    this.specular = specular;
  }
};

class mdl_t {
  constructor(subgroups, vertices)
  {
    this.subgroups = subgroups;
    this.vertices = vertices;
  }
};

function main()
{
  if (process.argv.length != 3) {
    console.log("usage:", path.parse(process.argv[1]).name, "[mdl]");
    process.exit(1);
  }
  
  const mdl_name = process.argv[2];
  
  const obj = obj_parse("obj/" + mdl_name + "/" + mdl_name + ".obj");
  const mdl = obj_to_mdl(obj);
  
  const write = new write_t();
  
  write_mdl(write, mdl, mdl_name);
}

function write_mdl(write, mdl, mdl_name)
{
  write.write_u32(mdl.subgroups.length);
  write.write_u32(mdl.vertices.length);
  
  for (const subgroup of mdl.subgroups) {
    write.write_s32(subgroup.material.diffuse);
    write.write_s32(subgroup.material.normal);
    write.write_vec3(subgroup.material.color);
    write.write_f32(subgroup.material.specular);
    write.write_u32(subgroup.offset);
    write.write_u32(subgroup.count);
  }
  
  for (const vertex of mdl.vertices) {
    write.write_vertex(vertex);
  }
  
  for (const subgroup of mdl.subgroups) {
    if (subgroup.material.diffuse != "") {
      const diffuse_path = "obj/" + mdl_name + "/" + subgroup.material.diffuse;
      const diffuse_copy = "../../assets/map/" + mdl_name + "/" + subgroup.material.diffuse;
      fs.copyFile(diffuse_path, diffuse_copy, (err) => console.log(err));
    }
    
    if (subgroup.material.normal != "") {
      const normal_path = "obj/" + mdl_name + "/" + subgroup.material.normal;
      const normal_copy = "../../assets/map/" + mdl_name + "/" + subgroup.material.normal;
      fs.copyFile(normal_path, normal_copy, (err) => console.log(err));
    }
  }
  
  fs.mkdir("../../assets/mdl/" + mdl_name, (err) => console.log(err));
  
  fs.writeFileSync("../../assets/mdl/" + mdl_name + "/" + mdl_name + ".mdl", Buffer.from(write.data()));
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
  const subgroups = [];
  
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
    
    const { diffuse, normal, color, specular } = obj.materials[material];
    const mdl_material = new mdl_material_t(diffuse, normal, color, specular);
    
    subgroups.push(new mdl_subgroup(mdl_material, offset, count));
  }
  
  return new mdl_t(subgroups, vertices);
}

main();
