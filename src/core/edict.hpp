#ifndef EDICT_HPP
#define EDICT_HPP

#include <memory>
#include <unordered_map>
#include <typeindex>

#define MAX_ENTITIES 256

using entity_t = int;
using signature_t = int;

class i_component_array_t {

};

template <typename T>
class component_array_t : public i_component_array_t {
private:
  std::array<T, MAX_ENTITIES> m_array;

public:
  T& get(entity_t entity) {
    return m_array[entity];
  }
};

class edict_t {
private:
  std::unordered_map<std::type_index, std::shared_ptr<i_component_array_t>> m_component_types;
  std::unordered_map<std::type_index, signature_t> m_component_signatures;
  std::array<signature_t, MAX_ENTITIES> m_entity_signatures;
  signature_t m_next_signature = 1;
  entity_t m_next_entity = 0;
  
  template <typename T>
  component_array_t<T>& get_component_array() {
    std::type_index component_type = std::type_index(typeid(T));
    assert(m_component_types.find(component_type) != m_component_types.end());
    return *std::static_pointer_cast<component_array_t<T>>(m_component_types[component_type]);
  }

public:
  entity_t add_entity() {
    assert(m_next_entity != MAX_ENTITIES);
    return m_next_entity++;
  }
  
  signature_t get_signature(entity_t entity) {
    assert(entity >= 0 && entity < MAX_ENTITIES);
    return m_entity_signatures[entity];
  }
  
  template <typename T>
  void bind_component() {
    m_component_types.insert({ std::type_index(typeid(T)), std::make_shared<component_array_t<T>>() });
    m_component_signatures.insert({ std::type_index(typeid(T)), m_next_signature });
    m_next_signature <<= 1;
  }
  
  template <typename T>
  T& enable_component(entity_t entity) {
    assert(entity >= 0 && entity < MAX_ENTITIES);
    component_array_t<T>& component_array = get_component_array<T>();
    m_entity_signatures[entity] |= m_component_signatures[std::type_index(typeid(T))];
    return component_array.get(entity);
  }
  
  template <typename T>
  T& get_component(entity_t entity) {
    assert(m_entity_signatures[entity] & m_component_signatures[std::type_index(typeid(T))] > 0);
    component_array_t<T>& component_array = get_component_array<T>();
    return component_array.get(entity);
  }
};

#endif
