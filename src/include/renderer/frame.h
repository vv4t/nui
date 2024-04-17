#ifndef FRAME_H
#define FRAME_H

frame_t frame_create();
void frame_input(int channel, texture_t t);
void frame_output(int channel, texture_t t);
void frame_destroy(frame_t f);

#endif
