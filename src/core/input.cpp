#include "input.hpp"
#include <iostream>

input_t::input_t() {
  std::fill(std::begin(m_axis), std::end(m_axis), 0.0);
  std::fill(std::begin(m_keys), std::end(m_keys), std::pair<int, int>(0, -1));
  m_num_keys = 0;
  m_move1 = -1;
  m_move2 = -1;
}

void input_t::bind_key(int axis, int key) {
  m_keys[m_num_keys++] = std::pair<int, int>(axis, key);
}

void input_t::bind_move(int axis1, int axis2) {
  m_move1 = axis1;
  m_move2 = axis2;
}

void input_t::key_event(int key, bool action) {
  for (int i = 0; i < m_num_keys; i++) {
    if (m_keys[i].second == key) {
      m_axis[m_keys[i].first] = action ? 1.0 : 0.0;
      break;
    }
  }
}

void input_t::move_event(float x, float y) {
  if (m_move1 >= 0) {
    m_axis[m_move1] = x;
  }
  
  if (m_move2 >= 0) {
    m_axis[m_move2] = y;
  }
}

float input_t::get_axis(int axis) const {
  return m_axis[axis];
}
