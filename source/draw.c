#include "draw.h"
#include "i2c.h"

#include <string.h>

void screen_init(void){
	void ARM11(void){
		__asm(".word 0xF10C01C0"); //CPSID AIF @ Disable Interrupts
		
		memset((void*)0x18300000, 0, 0x46500 + 0x38400);
		*((vu32*)0x10400468) = 0x18300000;
		*((vu32*)0x1040046c) = 0x18300000;
		*((vu32*)0x10400494) = 0x18300000;
		*((vu32*)0x10400498) = 0x18300000;
		*((vu32*)0x10400568) = 0x18346500;
		*((vu32*)0x1040056c) = 0x18346500;
		
		*((vu32*)0x1FFFFFF8) = 0;
		while(!*((vu32*)0x1FFFFFF8));
		((void (*)())*((vu32*)0x1FFFFFF8))();
	}
	
	*((vu32*)0x1FFFFFF8) = (u32)ARM11;
}

void screen_deinit(void){ //doesn't work, don't try it
	void ARM11(void){
		__asm(".word 0xF10C01C0");
		
		memset((void*)0x10400000, 0, 0x1000);
		*((vu32*)0x10141200) = 0;
		*((vu32*)0x10141204) = 0;
		
		*((vu32*)0x1FFFFFF8) = 0;
		while(!*((vu32*)0x1FFFFFF8));
		((void (*)())*((vu32*)0x1FFFFFF8))();
	}
	
	*((vu32*)0x1FFFFFF8) = (u32)ARM11;
}