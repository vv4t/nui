"use stirct";

import { plane_t } from "../shared/math.js";

const DOT_DEGREE = 0.01;

export class bsp_node_t {
  constructor(plane, hull)
  {
    this.plane = plane;
    this.behind = null;
    this.ahead = null;
    this.hull = hull;
  }
};

export class bsp_face_t {
  constructor(vertices, normal, plane, hull)
  {
    this.vertices = vertices;
    this.normal = normal;
    this.plane = plane;
    this.hull = hull;
  }
};

export function bsp_load(faces)
{
  return collapse_brush_R(faces);
}

function collapse_brush_R(faces)
{
  if (faces.length == 0) {
    return null;
  }
  
  const plane = face_to_plane(faces[0]);
  
  const [behind, middle, ahead] = split_brush(plane, faces);
  
  const node = new bsp_node_t(faces[0].plane, faces[0].hull);
  node.behind = collapse_brush_R(behind);
  node.ahead = collapse_brush_R(ahead);
  
  return node;
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
  const delta_pos = b.sub(a);
  const t = -(a.dot(plane.normal) - plane.distance) / delta_pos.dot(plane.normal);
  const pos = a.add(delta_pos.mulf(t));
  
  return pos;
}

function split_face_even(face, vbehind, vmiddle, vahead, plane)
{
  const shared = intersect_plane(vbehind, vahead, plane);
  
  const behind = new bsp_face_t([vbehind, shared, vmiddle], face.normal, face.plane, face.hull);
  const ahead = new bsp_face_t([vahead, shared, vmiddle], face.normal, face.plane, face.hull);
  
  return [ [behind], [], [ahead] ];
}

function split_face_uneven(face, vbase, vhead, plane, flip)
{
  const hit_a = intersect_plane(vhead, vbase[0], plane);
  const hit_b = intersect_plane(vhead, vbase[1], plane);
  
  const head = [ new bsp_face_t([hit_a, hit_b, vhead], face.normal, face.plane, face.hull) ];
  
  const base = [
    new bsp_face_t([vbase[0], vbase[1], hit_b], face.normal, face.plane, face.hull),
    new bsp_face_t([hit_b, hit_a, vbase[0]], face.normal, face.plane, face.hull)
  ];
  
  if (flip)
    return [head, [], base];
  else
    return [base, [], head];
}

function split_face(face, plane, debug)
{
  const behind = [];
  const middle = [];
  const ahead = [];
  
  for (const vertex of face.vertices) {
    const dist = vertex.dot(plane.normal) - plane.distance;
    
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
    if (face.normal.dot(plane.normal) < -1+DOT_DEGREE) {
      return [ [], [], [face] ];
    } else if (face.normal.dot(plane.normal) > 1-DOT_DEGREE) {
      return [ [], [face], [] ];
    } else {
      return [ [], [], [] ];
    }
  } else if (middle.length == 2) {
    if (behind.length > ahead.length)
      return [ [face], [], [] ];
    else
      return [ [], [], [face] ];
  } else if (middle.length == 1) {
    return split_face_even(face, behind[0], middle[0], ahead[0], plane);
  } else if (behind.length > ahead.length) {
    return split_face_uneven(face, behind, ahead[0], plane, false);
  } else if (behind.length < ahead.length) {
    return split_face_uneven(face, ahead, behind[0], plane, true);
  } else {
    throw "split_face(): unknown case";
  }
}

function face_to_plane(face)
{
  const n = face.normal;
  const d = face.normal.dot(face.vertices[0]);
  return new plane_t(n, d);
}
