"use strict";

import fs from "fs";
import path from "path";
import { write_t } from "../shared/write.js";
import { plane_t } from "../shared/math.js";
import { obj_parse } from "../shared/obj.js";
import { bsp_face_t, bsp_load } from "./bsp.js";

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

export class map_material_t {
  constructor(diffuse, color, specular)
  {
    this.diffuse = diffuse;
    this.color = color;
    this.specular = specular;
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

function map_load(obj)
{
  const map = new map_t();
  
  const materials = [];
  const bsp = [];
  const subgroups = {};
  
  for (let i = 0; i < obj.materials.length; i++) {
    subgroups[i] = [];
  }
  
  for (const object of obj.objects) {
    load_object(map, subgroups, bsp, object);
  }
  
  for (const [material, objects] of Object.entries(subgroups)) {
    const face_offset = map.faces.length;
    
    for (const faces of objects) {
      map.faces.push(...faces);
    }
    
    const face_count = map.faces.length - face_offset;
    
    const { diffuse, color, specular } = obj.materials[material];
    const map_material = new map_material_t(diffuse, color, specular);
    
    map.subgroups.push(new map_subgroup_t(map_material, face_offset, face_count));
  }
  
  map.nodes = flatten_bsp(bsp_load(bsp));
  
  return map;
}

function load_object(map, subgroups, bsp, object)
{
  const faces = [];
  
  const plane_map = {};
  const plane_offset = map.planes.length;
  
  const vertex_map = {};
  const vertex_offset = map.vertices.length;
  
  const hull = map.hulls.length;
  
  for (const face of object.faces) {
    load_face(faces, map, bsp, plane_map, vertex_map, hull, face);
  }
  
  const plane_count = map.planes.length - plane_offset;
  const vertex_count = map.vertices.length - vertex_offset;
  
  map.hulls.push(new map_hull_t(vertex_offset, vertex_count, plane_offset, plane_count));
  
  subgroups[object.material].push(faces);
}

function load_face(faces, map, bsp, plane_map, vertex_map, hull, face)
{
  const vertices = [];
  
  for (const vertex of face.vertices) {
    const id = `${vertex.pos.x} ${vertex.pos.y} ${vertex.pos.z}`;
    
    if (!vertex_map[id]) {
      vertex_map[id] = map.vertices.push(new map_vertex_t(vertex.pos)) - 1;
    }
    
    vertices.push(vertex_map[id]);
  }
  
  faces.push(new map_face_t(vertices[0], vertices[1], vertices[2], face.normal));
  
  const plane = face_to_plane(face);
  const id = `${plane.normal.x} ${plane.normal.y} ${plane.normal.z} ${plane.distance}`;
  
  if (!plane_map[id]) {
    plane_map[id] = map.planes.push(plane) - 1;
  }
  
  const bsp_vertices = [ face.vertices[0].pos, face.vertices[1].pos, face.vertices[2].pos ];
  
  bsp.push(new bsp_face_t(bsp_vertices, face.normal, plane_map[id], hull));
}

function map_write(map, map_name)
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
    write.write_vec3(subgroup.material.color);
    write.write_f32(subgroup.material.specular);
    write.write_u32(subgroup.face_offset);
    write.write_u32(subgroup.face_count);
  }
  
  for (const subgroup of map.subgroups) {
    if (subgroup.material.diffuse != "") {
      const diffuse_path = "obj/" + map_name + "/" + subgroup.material.diffuse;
      const diffuse_copy = "../../assets/map/" + map_name + "/" + subgroup.material.diffuse;
      
      fs.copyFile(diffuse_path, diffuse_copy, (err) => console.log(err));
    }
  }
  
  fs.mkdir("../../assets/map/" + map_name, (err) => console.log(err));
  
  fs.writeFileSync("../../assets/map/" + map_name + "/" + map_name + ".map", Buffer.from(write.data()));
}

function face_to_plane(face)
{
  const n = face.normal;
  const d = face.normal.dot(face.vertices[0].pos);
  return new plane_t(n, d);
}

function flatten_bsp(bsp)
{
  const nodes = [];
  flatten_bsp_R(nodes, bsp);
  return nodes;
}

function flatten_bsp_R(nodes, bsp)
{
  if (!bsp) {
    return -1;
  }
  
  const node = new map_bsp_node_t(bsp.plane, bsp.hull);
  const node_id = nodes.push(node) - 1;
  
  node.behind = flatten_bsp_R(nodes, bsp.behind);
  node.ahead = flatten_bsp_R(nodes, bsp.ahead);
  
  return node_id;
}

function main()
{
  if (process.argv.length != 3) {
    console.log("usage:", path.parse(process.argv[1]).name, "[map]");
    process.exit(1);
  }
  
  const name = process.argv[2];
  
  const obj = obj_parse("obj/" + name + "/" + name + ".obj");
  const map = map_load(obj);
  
  map_write(map, name);
}

main();
