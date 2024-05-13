#include <renderer/channel.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_ICHANNEL 8

typedef struct {
  char *name;
  GLuint type;
  texture_t texture;
  int disable;
} ichannel_t;

struct channel_s {
  ichannel_t ichannel[MAX_ICHANNEL];
  int num_ichannel;
};

channel_t channel_create()
{
  channel_t ch = malloc(sizeof(*ch));
  *ch = (struct channel_s) {0};
  return ch;
}

void channel_disable(channel_t ch, int channel)
{
  if (channel >= MAX_ICHANNEL) {
    return;
  }
  
  ch->ichannel[channel].disable = 1;
}

void channel_bind(channel_t ch)
{
  for (int i = 0; i < ch->num_ichannel; i++) {
    if (ch->ichannel[i].disable) {
      continue;
    }
    
    texture_bind(ch->ichannel[i].texture, ch->ichannel[i].type, i); 
  }
}

void channel_in(channel_t ch, const char *name, GLuint type, texture_t texture)
{
  int n = ch->num_ichannel++;
  while (ch->ichannel[n].disable) n = ch->num_ichannel++;
  
  if (n >= MAX_ICHANNEL) {
    return;
  }
  
  ch->ichannel[n].name = strdup(name);
  ch->ichannel[n].type = type;
  ch->ichannel[n].texture = texture;
}

void channel_shader_attach(shader_t shader, channel_t ch)
{
  shader_bind(shader);
  
  for (int i = 0; i < ch->num_ichannel; i++) {
    if (ch->ichannel[i].disable) {
      continue;
    }
    
    glUniform1i(glGetUniformLocation(shader, ch->ichannel[i].name), i);
  }
}

void channel_destroy(channel_t ch)
{
  for (int i = 0; i < ch->num_ichannel; i++) {
    if (ch->ichannel[i].disable) {
      continue;
    }
    
    free(ch->ichannel[i].name);
  }
  
  free(ch);
}

channel_t channel_copy(channel_t src)
{
  channel_t ch = malloc(sizeof(*ch));
  *ch = *src;
  
  for (int i = 0; i < ch->num_ichannel; i++) {
    if (ch->ichannel[i].disable) {
      continue;
    }
    
    ch->ichannel[i].name = strdup(src->ichannel[i].name);
  }
  
  return ch;
}
