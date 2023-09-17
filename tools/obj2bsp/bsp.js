"use strict";

import fs from "fs";
import path from "path";
import { write_t } from "../shared/write.js";
import { obj_parse, obj_vertex_t, obj_face_t } from "../shared/obj.js";

const DOT_DEGREE = 0.001;

function main()
{
  if (process.argv.length != 4) {
    console.log("usage:", path.parse(process.argv[1]).name, "[obj-file] [bsp-file]");
    process.exit(1);
  }
  
  const input_obj = process.argv[2];
  const out_bsp = process.argv[3];
  
  const obj = obj_parse(input_obj);
  const bsp = obj_to_bsp(obj);
  
  const write = new write_t();
  
  write_bsp(write, bsp, out_bsp);
}

function obj_to_bsp(obj)
{
  const faces = [];
  
  for (const object of obj.objects) {
    faces.push(...object.faces);
  }
  
  return collapse_brush_R(faces);
}

function write_bsp(write, bsp, out_bsp)
{
  const nodes = [];
  collapse_nodes_R(nodes, bsp);
  
  write.write_u32(nodes.length);
  
  for (const node of nodes) {
    write.write_vec3(node.plane.normal);
    write.write_f32(node.plane.distance);
    write.write_u32(node.behind);
    write.write_u32(node.ahead);
  }
  
  fs.writeFileSync(out_bsp, Buffer.from(write.data()));
}

function collapse_nodes_R(nodes, bsp)
{
  if (!bsp) {
    return -1;
  }
  
  const node = new bsp_node_t(bsp.plane);
  
  const node_id = nodes.length;
  nodes.push(node);
  
  node.behind = collapse_nodes_R(nodes, bsp.behind);
  node.ahead = collapse_nodes_R(nodes, bsp.ahead);
  
  return node_id;
}

class bsp_node_t {
  constructor(plane)
  {
    this.plane = plane;
    this.behind = null;
    this.ahead = null;
  }
};

class plane_t {
  constructor(normal, distance)
  {
    this.normal = normal;
    this.distance = distance;
  }
};

function collapse_brush_R(faces)
{
  if (faces.length == 0)
    return;
  
  let plane = face_to_plane(faces[0]);
  
  let [behind, ahead] = split_brush(faces, plane);
  
  const node = new bsp_node_t(plane);
  node.behind = collapse_brush_R(behind);
  node.ahead = collapse_brush_R(ahead);
  
  return node;
}

function face_to_plane(face)
{
  const n = face.normal;
  const d = face.normal.dot(face.vertices[0].pos);
  return new plane_t(n, d)
}

function split_brush(faces, plane)
{
  const behind = [];
  const middle = [];
  const ahead = [];
  
  for (const face of faces) {
    const [ f_behind, f_middle, f_ahead ] = split_face(face, plane);
    
    behind.push(...f_behind);
    middle.push(...f_middle);
    ahead.push(...f_ahead);
  }
  
  return [ behind, ahead ];
}

function intersect_plane(a, b, plane)
{
  const delta_pos = b.pos.sub(a.pos);
  const delta_uv = b.uv.sub(b.uv);
  
  const t = -(a.pos.dot(plane.normal) - plane.distance) / delta_pos.dot(plane.normal);
  
  const pos = a.pos.add(delta_pos.mulf(t));
  const uv = a.uv.add(delta_uv.mulf(t));
  
  return new obj_vertex_t(pos, uv);
}

function split_face_even(vbehind, vmiddle, vahead, plane, normal)
{
  const shared = intersect_plane(vbehind, vahead, plane);
  
  const behind = new obj_face_t([vbehind, shared, vmiddle], normal);
  const ahead = new obj_face_t([vahead, shared, vmiddle], normal);
  
  return [ [behind], [], [ahead] ];
}

function split_face_uneven(vbase, vhead, plane, normal, flip)
{
  const hit_a = intersect_plane(vhead, vbase[0], plane);
  const hit_b = intersect_plane(vhead, vbase[1], plane);
  
  const head = [ new obj_face_t([hit_a, hit_b, vhead], normal) ];
  
  const base = [
    new obj_face_t([vbase[0], vbase[1], hit_b], normal),
    new obj_face_t([hit_b, hit_a, vbase[0]], normal)
  ];
  
  if (flip)
    return [head, [], base];
  else
    return [base, [], head];
}

function split_face(face, plane)
{
  const behind = [];
  const middle = [];
  const ahead = [];
  
  for (const vertex of face.vertices) {
    const dist = vertex.pos.dot(plane.normal) - plane.distance;
    
    if (dist < -DOT_DEGREE) {
      behind.push(vertex);
    } else if (dist > +DOT_DEGREE) {
      ahead.push(vertex);
    } else {
      middle.push(vertex);
    }
  }
  
  if (behind.length == 3 || (behind.length == 2 && middle.length == 1)) {
    return [ [face], [], [] ];
  } else if (ahead.length == 3 || (ahead.length == 2 && middle.length == 1)) {
    return [ [], [], [face] ];
  } else if (middle.length == 3) {
    if (face.normal.dot(plane.normal) > +DOT_DEGREE)
      return [ [], [face], [] ];
    else
      return [ [], [], [face] ];
  } else if (middle.length == 2) {
    if (behind.length > ahead.length)
      return [ [face], [], [] ];
    else
      return [ [], [], [face] ];
  } else if (middle.length == 1) {
    return split_face_even(behind[0], middle[0], ahead[0], plane, face.normal);
  } else if (behind.length > ahead.length) {
    return split_face_uneven(behind, ahead[0], plane, face.normal, false);
  } else if (behind.length < ahead.length) {
    return split_face_uneven(ahead, behind[0], plane, face.normal, true);
  } else {
    throw "split_face(): unknown case";
  }
}

main();
