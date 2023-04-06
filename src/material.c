#include "material.h"

#include "texture.h"

bool material_init(material_t *material, const char *src_color, const char *src_normal)
{
  if (!texture_load(&material->color, src_color))
    return false;
  
  if (!texture_load(&material->normal, src_normal))
    return false;
  
  return true;
}
