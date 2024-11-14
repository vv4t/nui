#include "game.hpp"
#include <iostream>

game_t::game_t() : m_num_entities(0) {
  for (entity_t entity = 0; entity < MAX_ENTITIES; entity++) {
    m_components[entity] = HAS_NONE;
  }
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

transform_t& game_t::get_transform(entity_t entity) {
  return m_transforms[entity];
}

model_t& game_t::get_model(entity_t entity) {
  return m_models[entity];
}

bool game_t::has_component(entity_t entity, component_t component) {
  return (m_components[entity] & component) == component;
}

entity_t game_t::add_entity() {
  for (entity_t entity = 0; entity < m_num_entities; entity++) {
    if (m_components[entity] == HAS_NONE) {
      return entity;
    }
  }
  
  return m_num_entities++;
}

int game_t::count_entities() {
  return m_num_entities;
}
