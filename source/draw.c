#include "draw.h"

#include <string.h>

/*DEBUGGING*/
#define	K11COM_ADDRESS	*((vu32*)0x1FFF5000)
#define	ARM11_BUSY		0xDEADC0DE
#define	ENABLE_TOP_LCD	0x4C434469
#define	SET_SCREEN_BUF	0x42554646

void screen_init(void){ //Not sure if any of this works, can't test properly
	/*	The idea is to have arm11 control this */ 
	if (!*((u32*)0x10141204)){ //If the GPU isn't initialized (a9lh)
		*((u32*)0x10141204) |= 1; //Power on the GPU
		*((u32*)0x10141200) |= (1 << 16) | 1; //LCD init, GPU init
		
		K11COM_ADDRESS = ENABLE_TOP_LCD;
		while(K11COM_ADDRESS == ENABLE_TOP_LCD);
		while(K11COM_ADDRESS == ARM11_BUSY);
	} else {
		K11COM_ADDRESS = SET_SCREEN_BUF;
		while(K11COM_ADDRESS == SET_SCREEN_BUF);
		while(K11COM_ADDRESS == ARM11_BUSY);
	}
	
	memset((void*)0x18300000, 0xFF, 0x46500 + 0x38400); //Clear the framebuffer
}

void top_screen(void* top_buf){
	memcpy((void*)0x18300000, top_buf, 0x46500);
}

void bottom_screen(void* bottom_buf){
	memcpy((void*)0x18346500, bottom_buf, 0x38400);
}