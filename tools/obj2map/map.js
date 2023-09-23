"use strict";

import fs from "fs";
import path from "path";
import { write_t } from "../shared/write.js";
import { obj_parse, obj_vertex_t, obj_face_t } from "../shared/obj.js";

const DOT_DEGREE = 0.001;

class map_vertex_group_t {
  constructor(material, offset, count)
  {
    this.material = material;
    this.offset = offset;
    this.count = count;
  }
};

class map_vertex_t {
  constructor(pos, normal, uv)
  {
    this.pos = pos;
    this.normal = normal;
    this.uv = uv;
  }
};

class map_bsp_node_t {
  constructor(plane)
  {
    this.plane = plane;
    this.behind = -1;
    this.ahead = -1;
  }
};

class map_t {
  constructor(nodes, vertex_groups, vertices)
  {
    this.nodes = nodes;
    this.vertex_groups = vertex_groups;
    this.vertices = vertices;
  }
};

function main()
{
  if (process.argv.length != 3) {
    console.log("usage:", path.parse(process.argv[1]).name, "[map]");
    process.exit(1);
  }
  
  const map_name = process.argv[2];
  
  const obj = obj_parse("obj/" + map_name + "/" + map_name + ".obj");
  const map = obj_to_map(obj);
  
  const write = new write_t();
  
  write_map(write, map, map_name);
}

function obj_to_map(obj)
{
  const faces = [];
  
  for (const object of obj.objects) {
    faces.push(...object.faces);
  }
  
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
        vertices.push(new map_vertex_t(face.vertices[0].pos, face.normal, face.vertices[0].uv));
        vertices.push(new map_vertex_t(face.vertices[1].pos, face.normal, face.vertices[1].uv));
        vertices.push(new map_vertex_t(face.vertices[2].pos, face.normal, face.vertices[2].uv));
      }
    }
    
    const count = vertices.length - offset;
    
    vertex_groups.push(new map_vertex_group_t(obj.materials[material], offset, count));
  }
  
  const bsp = collapse_brush_R(faces, []);
  
  const nodes = [];
  flat_bsp_R(nodes, bsp);
  
  return new map_t(nodes, vertex_groups, vertices);
}

function write_map(write, map, map_name)
{
  write.write_u32(map.nodes.length);
  write.write_u32(map.vertex_groups.length);
  write.write_u32(map.vertices.length);
  
  for (const node of map.nodes) {
    write.write_vec3(node.plane.normal);
    write.write_f32(node.plane.distance);
    write.write_u32(node.behind);
    write.write_u32(node.ahead);
  }
  
  for (const vertex_group of map.vertex_groups) {
    write.write_s32("assets/map/" + map_name + "/" + vertex_group.material.diffuse);
    write.write_u32(vertex_group.offset);
    write.write_u32(vertex_group.count);
  }
  
  for (const vertex of map.vertices) {
    write.write_vertex(vertex);
  }
  
  for (const vertex_group of map.vertex_groups) {
    const diffuse_path = "obj/" + map_name + "/" + vertex_group.material.diffuse;
    const diffuse_copy = "../../assets/map/" + map_name + "/" + vertex_group.material.diffuse;
    
    fs.copyFile(diffuse_path, diffuse_copy, (err) => console.log(err));
  }
  
  fs.mkdir("../../assets/map/" + map_name, (err) => console.log(err));
  
  fs.writeFileSync("../../assets/map/" + map_name + "/" + map_name + ".map", Buffer.from(write.data()));
}

function flat_bsp_R(nodes, bsp)
{
  if (!bsp) {
    return -1;
  }
  
  const node = new map_bsp_node_t(bsp.plane);
  const node_id = nodes.push(node) - 1;
  
  node.behind = flat_bsp_R(nodes, bsp.behind);
  node.ahead = flat_bsp_R(nodes, bsp.ahead);
  
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

function collapse_brush_R(faces, hull)
{
  if (faces.length == 0) {
    let node = null;
    const bevels = do_bevel(hull);
    
    for (const bevel of bevels) {
      const new_node = new bsp_node_t(bevel);
      new_node.behind = node;
      node = new_node;
    }
    
    return node;
  }
  
  const plane = face_to_plane(faces[0]);
  
  const [behind, middle, ahead] = split_brush(plane, faces);
  const [b,m,a] = split_brush(plane, hull);
  
  const new_hull = [];
  new_hull.push(...middle);
  new_hull.push(...b);
  new_hull.push(...m);
  
  const node = new bsp_node_t(plane);
  
  node.behind = collapse_brush_R(behind, new_hull);
  node.ahead = collapse_brush_R(ahead, []);
  
  return node;
}

function do_bevel(hull)
{
  const bevels = [];
  
  for (let i = 0; i < hull.length; i++) {
    const face1 = hull[i];
    
    for (let j = i+1; j < hull.length; j++) {
      const face2 = hull[j];
      
      const shared = face1.vertices.filter(
        (v1) => face2.vertices.some(
          (v2) => {
            const delta = v1.pos.sub(v2.pos);
            return delta.dot(delta) < DOT_DEGREE;
          }
        )
      );
      
      if (shared.length === 2 && face1.normal.dot(face2.normal) < 0.5) {
        const normal = face1.normal.add(face2.normal).normalize();
        const distance = shared[0].pos.dot(normal);
        
        bevels.push(new plane_t(normal, distance));
      }
    }
  }
  
  return bevels;
}

function face_to_plane(face)
{
  const n = face.normal;
  const d = face.normal.dot(face.vertices[0].pos);
  return new plane_t(n, d)
}

function split_brush(plane, faces)
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
  
  return [ behind, middle, ahead ];
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
