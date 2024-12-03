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
  MESHNAME_PLANE,
  MESHNAME_CUBOID,
  MAX_MESHNAME
};

class model_t {
public:
  meshname_t meshname;
  
  inline model_t() {
    meshname = MESHNAME_PLANE;
  }
  
  inline model_t(meshname_t _meshname) {
    meshname = _meshname;
  }
};

enum component_t {
  HAS_NONE = 0,
  HAS_TRANSFORM = (1 << 0),
  HAS_MODEL = (1 << 1)
};

inline component_t operator|(component_t a, component_t b) {
  return static_cast<component_t>(static_cast<int>(a) | static_cast<int>(b));
}

static const int MAX_ENTITIES = 256;

class game_t {
private:
  transform_t m_transforms[MAX_ENTITIES];
  model_t m_models[MAX_ENTITIES];
  component_t m_components[MAX_ENTITIES];
  int m_num_entities;
  entity_t m_camera;

public:
  game_t();
  
  entity_t get_camera();
  entity_t add_entity();
  
  void update(input_t& input);
  int entity_count();
  bool has_component(entity_t entity, component_t components);
  
  transform_t& enable_transform(entity_t entity, transform_t transform);
  model_t& enable_model(entity_t entity, model_t model);
  
  transform_t& get_transform(entity_t entity);
  model_t& get_model(entity_t entity);
};

#endif
