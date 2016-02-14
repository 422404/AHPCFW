#include "draw.h"
#include "standard.h"

#define TOP_LEFT	((u8*)(*(u32*)((u32)0x080FFFC0 + 4*((*(vu32*)0x080FFFD8)&1))))
#define TOP_RIGHT	((u8*)(*(u32*)((u32)0x080FFFC8 + 4*((*(vu32*)0x080FFFD8)&1))))
#define BOTTOM		((u8*)(*(u32*)0x080FFFD0 + 4*((*(vu32*)0x080FFFDC)&1)))

int top_screen(u8* left_buf, u8* right_buf){
	_memcpy(TOP_LEFT, left_buf, 0x46500);
	if (right_buf != NULL) _memcpy(TOP_RIGHT, right_buf, 0x46500);
	
	return 0;
}

int bottom_screen(u8* bottom_buf){
	_memcpy(BOTTOM, bottom_buf, 0x38400);
	
	return 0;
}