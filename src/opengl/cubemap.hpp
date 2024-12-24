#ifndef CUBEMAP_HPP
#define CUBEMAP_HPP

class cubemap_t {
private:
  // GLuint m_texture;

public:
  cubemap_t();
  void bind(int channel);
};

#endif
