#include "game.hpp"
#include <iostream>

game_t::game_t() : m_num_entities(0) {
  for (entity_t entity = 0; entity < MAX_ENTITIES; entity++) {
    m_components[entity] = HAS_NONE;
  }
  
  {
    entity_t entity = add_entity();
    transform_t& transform = enable_transform(entity, transform_t());
      transform.position = vec3(0, 0, -3);
    enable_aabb_collider(entity, aabb_collider_t(vec3(-0.5), vec3(0.5)));
    bind_character_body(entity);
    m_camera = entity;
  }
}

void game_t::update(input_t& input) {
  control_character_movement(input);
  integrate_character_velocity();
  resolve_character_collision();
}

void game_t::control_character_movement(input_t& input) {
  character_body_t& character_body = get_character_body();
  
  if (!has_component(character_body.entity, HAS_TRANSFORM)) return;
  
  transform_t &transform = get_transform(character_body.entity);
  transform.rotation = vec3(-input.get_axis(1), -input.get_axis(0), 0.0);
  character_body.velocity = vec3();
  
  if (input.get_axis(2)) {
    character_body.velocity += (mat4::rotate_xyz(transform.rotation) * vec4(0, 0, 1, 1)).get_xyz();
  }
  
  if (input.get_axis(3)) {
    character_body.velocity += (mat4::rotate_xyz(transform.rotation) * vec4(-1, 0, 0, 1)).get_xyz();
  }
  
  if (input.get_axis(4)) {
    character_body.velocity += (mat4::rotate_xyz(transform.rotation) * vec4(0, 0, -1, 1)).get_xyz();
  }
  
  if (input.get_axis(5)) {
    character_body.velocity += (mat4::rotate_xyz(transform.rotation) * vec4(1, 0, 0, 1)).get_xyz();
  }
}

void game_t::integrate_character_velocity() {
  character_body_t character_body = get_character_body();
  
  if (!has_component(character_body.entity, HAS_TRANSFORM)) return;
  
  transform_t &transform = get_transform(character_body.entity);
  transform.position += character_body.velocity * 0.05;
}

vec3 get_aabb_collision_vector(vec3 bound_a, vec3 bound_b) {
  vec3 axis_a = bound_a.reduce_to_min_axis();
  vec3 axis_b = bound_b.reduce_to_min_axis();
  
  if (axis_a.length_squared() < axis_b.length_squared()) {
    return axis_a;
  } else {
    return -axis_b;
  }
}

void game_t::resolve_character_collision() {
  character_body_t character_body = get_character_body();
  
  if (!has_component(character_body.entity, HAS_TRANSFORM)) return;
  
  transform_t& transform = get_transform(character_body.entity);
  aabb_collider_t& aabb_collider = get_aabb_collider(character_body.entity);
  
  for (entity_t entity = 0; entity < m_num_entities; entity++) {
    if (!has_component(entity, HAS_TRANSFORM | HAS_AABB_COLLIDER)) continue;
    if (entity == character_body.entity) continue;
    
    transform_t& entity_transform = get_transform(entity);
    aabb_collider_t& entity_aabb_collider = get_aabb_collider(entity);
    
    vec3 a_min = transform.position + aabb_collider.a;
    vec3 a_max = transform.position + aabb_collider.b;
    vec3 b_min = entity_transform.position + entity_aabb_collider.a;
    vec3 b_max = entity_transform.position + entity_aabb_collider.b;
    
    vec3 bound_a = b_max - a_min;
    vec3 bound_b = a_max - b_min;
    
    if (bound_a.is_positive() && bound_b.is_positive()) {
      vec3 collision_vector = get_aabb_collision_vector(bound_a, bound_b);
      vec3 normal = collision_vector.normalize();
      
      float velocity_resolution = -vec3::dot(normal, character_body.velocity);
      
      transform.position += collision_vector * 1.01;
      character_body.velocity += normal * velocity_resolution;
    }
  }
}

entity_t game_t::get_camera() {
  return m_camera;
}

entity_t game_t::add_entity() {
  for (entity_t entity = 0; entity < m_num_entities; entity++) {
    if (m_components[entity] == HAS_NONE) {
      return entity;
    }
  }
  
  return m_num_entities++;
}

bool game_t::has_component(entity_t entity, component_t component) {
  return (m_components[entity] & component) == component;
}

int game_t::entity_count() {
  return m_num_entities;
}

character_body_t& game_t::bind_character_body(entity_t entity) {
  m_character_body.entity = entity;
  return m_character_body;
}

transform_t& game_t::enable_transform(entity_t entity, transform_t transform) {
  m_components[entity] = m_components[entity] | HAS_TRANSFORM;
  m_transforms[entity] = transform;
  return m_transforms[entity];
}

model_t& game_t::enable_model(entity_t entity, model_t model) {
  m_components[entity] = m_components[entity] | HAS_MODEL;
  m_models[entity] = model;
  return m_models[entity];
}

aabb_collider_t& game_t::enable_aabb_collider(entity_t entity, aabb_collider_t aabb_collider) {
  m_components[entity] = m_components[entity] | HAS_AABB_COLLIDER;
  m_aabb_colliders[entity] = aabb_collider;
  return m_aabb_colliders[entity];
}

character_body_t& game_t::get_character_body() {
  return m_character_body;
}

transform_t& game_t::get_transform(entity_t entity) {
  return m_transforms[entity];
}

model_t& game_t::get_model(entity_t entity) {
  return m_models[entity];
}

aabb_collider_t& game_t::get_aabb_collider(entity_t entity) {
  return m_aabb_colliders[entity];
}
