#ifndef DRAW_H
#define DRAW_H

#include "types.h"

int top_screen(u8* left_buf, u8* right_buf);
int bottom_screen(u8* bottom_buf);
void color_code(u8 r, u8 g, u8 b);

#endif