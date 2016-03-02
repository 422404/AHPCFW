#ifndef DRAW_H
#define DRAW_H

#include "types.h"

void screen_init(void);
void screen_deinit(void);
void screen_reinit(void);
void flip_top_buffers(void);
void flip_bottom_buffers(void);
void draw_top_screen(void* src);
void draw_bottom_screen(void* src);
void clear_framebuffers(void);

#endif