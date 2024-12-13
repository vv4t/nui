#ifndef GAME_H
#define GAME_H

#include <util/math3d.hpp>
#include <core/input.hpp>

using entity_t = int;

class transform_t {
public:
  vec3 position;
  vec3 rotation;
  vec3 scale;
  
  inline transform_t() {}
  
  void move_to(vec3 _position) {
    position = _position;
  }
  
  void rotate_to(vec3 _rotation) {
    rotation = _rotation;
  }
  
  void scale_to(vec3 _scale) {
    scale = _scale;
  }
};

enum meshname_t {
  MESH_PLANE,
  MESH_CUBOID
};

enum materialname_t {
  MATERIAL_DEFAULT,
  MATERIAL_BRICK,
  MATERIAL_GRASS
};

class model_t {
public:
  meshname_t mesh;
  materialname_t material;
  
  inline model_t(meshname_t _mesh, materialname_t _material) {
    mesh = _mesh;
    material = _material;
  }
  
  inline model_t() : model_t(MESH_CUBOID, MATERIAL_DEFAULT) {}
};

class aabb_t {
public:
  vec3 a, b;
  
  inline aabb_t(vec3 _a, vec3 _b) {
    a = _a;
    b = _b;
  }
  
  inline aabb_t() : aabb_t(vec3(0.0), vec3(1.0)) {}
};

class character_body_t {
public:
  entity_t entity;
  vec3 velocity;
  bool is_grounded;
  
  character_body_t() {
    is_grounded = false;
  }
};

enum component_t {
  HAS_NONE        = 0,
  HAS_TRANSFORM   = (1 << 0),
  HAS_MODEL       = (1 << 1),
  HAS_AABB        = (1 << 2)
};

inline component_t operator|(component_t a, component_t b) {
  return static_cast<component_t>(static_cast<int>(a) | static_cast<int>(b));
}

static const int MAX_ENTITIES = 256;

class game_t {
private:
  transform_t m_transforms[MAX_ENTITIES];
  model_t m_models[MAX_ENTITIES];
  aabb_t m_aabbs[MAX_ENTITIES];
  component_t m_components[MAX_ENTITIES];
  character_body_t m_character_body;
  int m_num_entities;
  entity_t m_camera;
  
  void resolve_character_collision();
  void control_character_movement(input_t& input);
  void integrate_character_velocity();

public:
  game_t();
  
  entity_t get_camera();
  
  void update(input_t& input);
  
  entity_t add_entity();
  int entity_count();
  bool has_component(entity_t entity, component_t components);
  
  character_body_t& bind_character_body(entity_t entity);
  
  transform_t& enable_transform(entity_t entity, transform_t transform);
  model_t& enable_model(entity_t entity, model_t model);
  aabb_t& enable_aabb(entity_t entity, aabb_t aabb);
  
  character_body_t& get_character_body();
  transform_t& get_transform(entity_t entity);
  model_t& get_model(entity_t entity);
  aabb_t& get_aabb(entity_t entity);
};

#endif
