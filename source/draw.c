#include "draw.h"
#include "i2c.h"

#include <string.h>

void screen_init(void){
	void ARM11(void){
		if (!*((vu32*)0x10202014)){ //Credit to b1|1s, mid-kid, Normmatt, dark_samus, and everyone else involved for screen init
			/* PDN_GPU_CNT */
			*((vu32*)0x10141200) = 0x10000;
			for(int i = 0xC; i > 0; i-=2); //delay
			*((vu32*)0x10141200) |= 0x7F;
			
			/* Main Framebuffer Setup */
			*((vu32*)0x10400400) = 0x1c2;
			*((vu32*)0x10400404) = 0xd1;
			*((vu32*)0x10400408) = 0x1c1;
			*((vu32*)0x1040040c) = 0x1c1;
			*((vu32*)0x10400414) = 0xcf;
			*((vu32*)0x10400418) = 0xd1;
			*((vu32*)0x1040041c) = 0x1c501c1;
			*((vu32*)0x10400420) = 0x10000;
			*((vu32*)0x10400424) = 0x19d;
			*((vu32*)0x10400428) = 2;
			*((vu32*)0x1040042c) = 0x192;
			*((vu32*)0x10400430) = 0x192;
			*((vu32*)0x10400434) = 0x192;
			*((vu32*)0x10400438) = 1;
			*((vu32*)0x1040043c) = 2;
			*((vu32*)0x10400440) = 0x1960192;
			*((vu32*)0x1040045C) = 0xf00190;
			*((vu32*)0x10400460) = 0x1c100d1;
			*((vu32*)0x10400464) = 0x1920002;
			*((vu32*)0x10400470) = 0x80341;
			*((vu32*)0x10400474) = 0x10501;
			for (int i = 0; i < 256; i++) *((vu32*)0x10400484) = 0x10101 * i;
			*((vu32*)0x10400490) = 0x2D0;
			
			/* Sub Framebuffer Setup */
			*((vu32*)0x10400500) = 0x1c2;
			*((vu32*)0x10400504) = 0xd1;
			*((vu32*)0x10400508) = 0x1c1;
			*((vu32*)0x1040050c) = 0x1c1;
			*((vu32*)0x10400510) = 0xcd;
			*((vu32*)0x10400514) = 0xcf;
			*((vu32*)0x10400518) = 0xd1;
			*((vu32*)0x1040051c) = 0x1c501c1;
			*((vu32*)0x10400520) = 0x10000;
			*((vu32*)0x10400524) = 0x19d;
			*((vu32*)0x10400528) = 0x52;
			*((vu32*)0x1040052c) = 0x192;
			*((vu32*)0x10400530) = 0x192;
			*((vu32*)0x10400534) = 0x4f;
			*((vu32*)0x10400538) = 0x50;
			*((vu32*)0x1040053c) = 0x52;
			*((vu32*)0x10400540) = 0x1980194;
			*((vu32*)0x10400548) = 0x11;
			*((vu32*)0x1040055C) = 0xf00140;
			*((vu32*)0x10400560) = 0x1c100d1;
			*((vu32*)0x10400564) = 0x1920052;
			*((vu32*)0x10400570) = 0x80301;
			*((vu32*)0x10400574) = 0x10501;
			for (int i = 0; i < 256; i++) *((vu32*)0x10400584) = 0x10101 * i;
			*((vu32*)0x10400590) = 0x2D0;
			
			/* Framebuffers */
			*((vu32*)0x10400468) = 0x18300000;
			*((vu32*)0x1040046c) = 0x18400000;
			*((vu32*)0x10400568) = 0x18346500;
			*((vu32*)0x1040056c) = 0x18446500;
			
			/* Reg. Writes */
			*((vu32*)0x10202204) = 0;
			*((vu32*)0x10202a04) = 0;
			*((vu32*)0x1020200C) &= 0xFFFEFFFE;
			
			*((vu32*)0x10400004) = 0x70100;
			*((vu32*)0x10400030) &= 0xFFFFF0FF;
			
			*((vu32*)0x10401000) = 0;
			*((vu32*)0x10401080) = 0x12345678;
			*((vu32*)0x104010C0) = 0xFFFFFFF0;
			*((vu32*)0x104010D0) = 1;
			
			*((vu32*)0x1040001C) &= 0xFFFFFFFD;
			*((vu32*)0x1040002C) &= 0xFFFFFFFD;
			*((vu32*)0x10400050) = 0x22221200;
			*((vu32*)0x10400054) = 0xFF2;
			
			*((vu32*)0x10202240) = 0x39;
			*((vu32*)0x10202A40) = 0x39;
			
			/* Backlight + LCD Init */
			i2cWriteRegister(I2C_DEV_MCU, 0x22, 0x2A);
			*((vu32*)0x10202014) = 1;
			*((vu32*)0x10202244) = 0x1023E;
			*((vu32*)0x10202A44) = 0x1023E;
		} else { //if screen is already initialized, just set the buffers
			*((vu32*)0x10400468) = 0x18300000;
			*((vu32*)0x1040046c) = 0x18400000;
			*((vu32*)0x10400568) = 0x18346500;
			*((vu32*)0x1040056c) = 0x18446500;
		}
		
		*((vu32*)0x1FFFFFF8) = 0;
		while(!*((vu32*)0x1FFFFFF8));
		((void (*)())*((vu32*)0x1FFFFFF8))();
	}
	
	*((vu32*)0x1FFFFFF8) = (u32)ARM11;
	while(*((vu32*)0x1FFFFFF8));
}

void screen_deinit(void){
	void ARM11(void){
		*((vu32*)0x10202244) = 0;
		*((vu32*)0x10202A44) = 0;
		*((vu32*)0x10202014) = 0;
		
		*((vu32*)0x1FFFFFF8) = 0;
		while(!*((vu32*)0x1FFFFFF8));
		((void (*)())*((vu32*)0x1FFFFFF8))();
	}
	
	*((vu32*)0x1FFFFFF8) = (u32)ARM11;
	while(*((vu32*)0x1FFFFFF8));
}

void screen_reinit(void){
	void ARM11(void){
		i2cWriteRegister(I2C_DEV_MCU, 0x22, 0x2A);
		*((vu32*)0x10202014) = 1;
		*((vu32*)0x10202244) = 0x1023E;
		*((vu32*)0x10202A44) = 0x1023E;
		
		*((vu32*)0x1FFFFFF8) = 0;
		while(!*((vu32*)0x1FFFFFF8));
		((void (*)())*((vu32*)0x1FFFFFF8))();
	}
	
	*((vu32*)0x1FFFFFF8) = (u32)ARM11;
	while(*((vu32*)0x1FFFFFF8));
}

void flip_top_buffers(void){
	void ARM11(void){
		*((vu32*)0x10400478) ^= 1;
		
		*((vu32*)0x1FFFFFF8) = 0;
		while(!*((vu32*)0x1FFFFFF8));
		((void (*)())*((vu32*)0x1FFFFFF8))();
	}
	
	*((vu32*)0x1FFFFFF8) = (u32)ARM11;
	while(*((vu32*)0x1FFFFFF8));
}

void flip_bottom_buffers(void){
	void ARM11(void){
		*((vu32*)0x10400578) ^= 1;
		
		*((vu32*)0x1FFFFFF8) = 0;
		while(!*((vu32*)0x1FFFFFF8));
		((void (*)())*((vu32*)0x1FFFFFF8))();
	}
	
	*((vu32*)0x1FFFFFF8) = (u32)ARM11;
	while(*((vu32*)0x1FFFFFF8));
}

void draw_top_screen(void* src){
	void ARM11(void){
		*((vu32*)0x23EFFFFC) = *((vu32*)0x10400478);
		
		*((vu32*)0x1FFFFFF8) = 0;
		while(!*((vu32*)0x1FFFFFF8));
		((void (*)())*((vu32*)0x1FFFFFF8))();
	}
	
	*((vu32*)0x1FFFFFF8) = (u32)ARM11;
	while(*((vu32*)0x1FFFFFF8));
	
	if (*((vu32*)0x23EFFFFC) & 1){
		memcpy((void*)0x18300000, src, 0x46500);
	} else {
		memcpy((void*)0x18400000, src, 0x46500);
	}
	flip_top_buffers();
}

void draw_bottom_screen(void* src){
	void ARM11(void){
		*((vu32*)0x23EFFFFC) = *((vu32*)0x10400578);
		
		*((vu32*)0x1FFFFFF8) = 0;
		while(!*((vu32*)0x1FFFFFF8));
		((void (*)())*((vu32*)0x1FFFFFF8))();
	}
	
	*((vu32*)0x1FFFFFF8) = (u32)ARM11;
	while(*((vu32*)0x1FFFFFF8));
	
	if (*((vu32*)0x23EFFFFC) & 1){
		memcpy((void*)0x18346500, src, 0x38400);
	} else {
		memcpy((void*)0x18446500, src, 0x38400);
	}
	flip_bottom_buffers();
}

void clear_framebuffers(void){
	memset((void*)0x18300000, 0, 0x46500 + 0x38400);
	memset((void*)0x18400000, 0, 0x46500 + 0x38400);
}
