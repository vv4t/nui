#include "gl.h"

#include "log.h"

bool gl_init()
{
  glewExperimental = true;
  
  GLenum status = glewInit();
  if (status != GLEW_OK)
    LOG_ERROR("failed to initialize GLEW: %s", glewGetErrorString(status));
  
  return status == GLEW_OK;
}
