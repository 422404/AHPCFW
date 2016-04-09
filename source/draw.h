#ifndef DRAW_H
#define DRAW_H

#include "types.h"

#define TopDrawBuffer (u8*)0x18500000
#define BotDrawBuffer (u8*)0x18546500

#define TopFrameBuf0  (u8*)0x18300000
#define TopFrameBuf1  (u8*)0x18400000
#define BotFrameBuf0  (u8*)0x18346500
#define BotFrameBuf1  (u8*)0x18446500

void screen_init(void);
void screen_deinit(void);
void screen_reinit(void);

void flip_top_buffers(void);
void flip_bottom_buffers(void);
void update_top_screen(void);
void update_bottom_screen(void);

void clear_framebuffers(void);

#endif