#ifndef DRAW_H
#define DRAW_H

#include "types.h"

#define TopDrawBuffer (u8*)0x18500000
#define BotDrawBuffer (u8*)0x18546500

#define TopFrameBuf0  (u8*)0x18300000
#define TopFrameBuf1  (u8*)0x18400000
#define BotFrameBuf0  (u8*)0x18346500
#define BotFrameBuf1  (u8*)0x18446500

#define ARM11Pointer  *((vu32 *)0x23EFFFF8)
#define FramebufSel   *((vu32 *)0x23EFFFFC)
#define ARM11Entry    *((vu32 *)0x1FFFFFF8)

void screen_init(void);
void screen_deinit(void);
void screen_reinit(void);

void flip_top_buffers(void);
void flip_bottom_buffers(void);
void draw_top_screen(void);
void draw_bottom_screen(void);

void clear_framebuffers(void);

#endif