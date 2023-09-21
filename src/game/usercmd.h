#ifndef USERCMD_H
#define USERCMD_H

typedef struct {
  int   forward;
  int   back;
  int   left;
  int   right;
  
  float d_yaw;
  float d_pitch;
} usercmd_t;

#endif
