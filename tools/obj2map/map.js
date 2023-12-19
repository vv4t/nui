"use strict";

import fs from "fs";
import path from "path";
import { write_t } from "../shared/write.js";

export class map_vertex_t {
  constructor(pos)
  {
    this.pos = pos;
  }
};

export class map_face_t {
  constructor(v1, v2, v3, normal)
  {
    this.v1 = v1;
    this.v2 = v2;
    this.v3 = v3;
    this.normal = normal;
  }
};

export class map_hull_t {
  constructor(vertex_offset, vertex_count, plane_offset, plane_count)
  {
    this.vertex_offset = vertex_offset;
    this.vertex_count = vertex_count;
    this.plane_offset = plane_offset;
    this.plane_count = plane_count;
  }
};

export class map_subgroup_t {
  constructor(material, face_offset, face_count)
  {
    this.material = material;
    this.face_offset = face_offset;
    this.face_count = face_count;
  }
};

export class map_bsp_node_t {
  constructor(plane, hull)
  {
    this.plane = plane;
    this.behind = -1;
    this.ahead = -1;
    this.hull = hull;
  }
};

export class map_t {
  constructor()
  {
    this.vertices = [];
    this.planes = [];
    this.faces = [];
    this.hulls = [];
    this.nodes = [];
    this.subgroups = [];
  }
};

export function write_map(map, map_name)
{
  const write = new write_t();
  
  write.write_u32(map.vertices.length);
  write.write_u32(map.planes.length);
  write.write_u32(map.faces.length);
  write.write_u32(map.hulls.length);
  write.write_u32(map.nodes.length);
  write.write_u32(map.subgroups.length);
  
  for (const vertex of map.vertices) {
    write.write_vec3(vertex.pos);
  }
  
  for (const plane of map.planes) {
    write.write_vec3(plane.normal);
    write.write_f32(plane.distance);
  }
  
  for (const face of map.faces) {
    write.write_u32(face.v1);
    write.write_u32(face.v2);
    write.write_u32(face.v3);
    write.write_vec3(face.normal);
  }
  
  for (const hull of map.hulls) {
    write.write_u32(hull.vertex_offset);
    write.write_u32(hull.vertex_count);
    write.write_u32(hull.plane_offset);
    write.write_u32(hull.plane_count);
  }
  
  for (const node of map.nodes) {
    write.write_u32(node.plane);
    write.write_u32(node.behind);
    write.write_u32(node.ahead);
    write.write_u32(node.hull);
  }
  
  for (const subgroup of map.subgroups) {
    write.write_s32(subgroup.material.diffuse);
    write.write_u32(subgroup.face_offset);
    write.write_u32(subgroup.face_count);
  }
  
  for (const subgroup of map.subgroups) {
    const diffuse_path = "obj/" + map_name + "/" + subgroup.material.diffuse;
    const diffuse_copy = "../../assets/map/" + map_name + "/" + subgroup.material.diffuse;
    
    fs.copyFile(diffuse_path, diffuse_copy, (err) => console.log(err));
  }
  
  fs.mkdir("../../assets/map/" + map_name, (err) => console.log(err));
  
  fs.writeFileSync("../../assets/map/" + map_name + "/" + map_name + ".map", Buffer.from(write.data()));
}
