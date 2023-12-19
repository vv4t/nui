"use strict";

import { plane_t } from "../shared/math.js";
import { obj_parse } from "../shared/obj.js";
import { bsp_face_t, bsp_load } from "./bsp.js";
import { write_map, map_t, map_vertex_t, map_face_t, map_hull_t, map_subgroup_t, map_bsp_node_t } from "./map.js";

function main()
{
  if (process.argv.length != 3) {
    console.log("usage:", path.parse(process.argv[1]).name, "[map]");
    process.exit(1);
  }
  
  const map_name = process.argv[2];
  
  const obj = obj_parse("obj/" + map_name + "/" + map_name + ".obj");
  const map = map_load(obj);
  
  write_map(map, map_name);
}

function map_load(obj)
{
  const map = new map_t();
  
  map.vertices = [];
  map.planes = [];
  map.faces = [];
  map.hulls = [];
  map.nodes = [];
  map.subgroups = [];
  
  const bsp_faces = [];
  const subgroups = {};
  
  for (const object of obj.objects) {
    const faces = [];
    
    const plane_map = {};
    const plane_offset = map.planes.length;
    
    const vertex_map = {};
    const vertex_offset = map.vertices.length;
    
    for (const face of object.faces) {
      const map_face = [];
      
      for (const vertex of face.vertices) {
        const id = `${vertex.pos.x} ${vertex.pos.y} ${vertex.pos.z}`;
        
        let idx = vertex_map[id];
        
        if (!idx) {
          idx = map.vertices.push(new map_vertex_t(vertex.pos)) - 1;
          vertex_map[id] = idx;
        }
        
        map_face.push(idx);
      }
      
      faces.push(new map_face_t(map_face[0], map_face[1], map_face[2], face.normal));
      
      const plane = face_to_plane(face);
      
      const id = `${plane.normal.x} ${plane.normal.y} ${plane.normal.z} ${plane.distance}`;
      let idx = plane_map[id];
      
      if (!idx) {
        idx = map.planes.push(plane) - 1;
        plane_map[id] = idx;
      }
      
      const bsp_vertices = [ face.vertices[0].pos, face.vertices[1].pos, face.vertices[2].pos ];
      const hull = map.hulls.length;
      
      bsp_faces.push(new bsp_face_t(bsp_vertices, face.normal, idx, hull));
    }
    
    const plane_count = map.planes.length - plane_offset;
    const vertex_count = map.vertices.length - vertex_offset;
    
    map.hulls.push(new map_hull_t(vertex_offset, vertex_count, plane_offset, plane_count));
    
    if (object.material in subgroups) {
      subgroups[object.material].push(faces);
    } else {
      subgroups[object.material] = [faces];
    }
  }
  
  for (const [material, objects] of Object.entries(subgroups)) {
    const face_offset = map.faces.length;
    
    for (const faces of objects) {
      map.faces.push(...faces);
    }
    
    const face_count = map.faces.length - face_offset;
    
    map.subgroups.push(new map_subgroup_t(obj.materials[material], face_offset, face_count));
  }
  
  map.nodes = flatten_bsp(bsp_load(bsp_faces));
  
  return map;
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

main();
