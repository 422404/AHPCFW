#include "draw.h"

#include <string.h>

void screen_init(void){
	/*	The idea is to have arm11 control this, setting each top screen buf to 0x18300000:
	*((u32*)0x1EF00468) = 0x18300000;
	*((u32*)0x1EF0046C) = 0x18300000;
	*((u32*)0x1EF00494) = 0x18300000;
	*((u32*)0x1EF00498) = 0x18300000;
	
		And each bottom screen buf to 0x18346500: (Although, I'm not so sure I'll be using the bottom screen)
	*((u32*)0x1EF00568) = 0x18346500;
	*((u32*)0x1EF0056C) = 0x18346500;
	*/
	
	/*	For arm9loaderhax it's a bit more complex... This can be done through arm9:
	*((u32*)0x10141204) |= 1; //Power on the GPU
	*((u32*)0x10141200) |= (1 << 16) | 1; //LCD init, GPU init
	
		While the following would be arm11:
	*((u32*)0x10202240) = 0xFFFFFFFF; //Enable LCD backlight? Needs to be tested...
	*((u32*)0x104000C0) = 0x20000000; //Power on the backlight
	
	*((u32*)0x10400030) &= 0xFFFFF0FF;
	*((u32*)0x10400004) = 0x70100;
	*((u32*)0x1040001C) &= 0xFFFFFFFD;
	*((u32*)0x1040002C) &= 0xFFFFFFFD;
	*((u32*)0x10400050) = 0x22221200;
	*((u32*)0x10400054) = 0xFF2;
	*((u32*)0x10401000) = 0;
	*((u32*)0x10401080) = 0x12345678;
	*((u32*)0x104010C0) = 0xFFFFFFF0;
	*((u32*)0x104010D0) = 1;
	*((u32*)0x10400C18) &= 0xFFFFFEFF;
	
	// Initialize the Top Screen Framebuffer Setup
	*((u32*)0x10400400) = 0x1C2;
	*((u32*)0x10400404) = 0xD1;
	*((u32*)0x10400408) = 0x1C1;
	*((u32*)0x1040040C) = 0x1C1;
	*((u32*)0x10400410) = 0;
	*((u32*)0x10400414) = 0xCF;
	*((u32*)0x10400418) = 0xD1;
	*((u32*)0x1040041C) = 0x1C501C1;
	*((u32*)0x10400420) = 0x10000;
	*((u32*)0x10400424) = 0x19D;
	*((u32*)0x10400428) = 2;
	*((u32*)0x1040042C) = 0x1C2;
	*((u32*)0x10400430) = 0x1C2;
	*((u32*)0x10400434) = 0x1C2;
	*((u32*)0x10400438) = 1;
	*((u32*)0x1040043C) = 2;
	*((u32*)0x10400440) = 0x1960192;
	*((u32*)0x10400444) = 0;
	*((u32*)0x10400448) = 0;
	*((u32*)0x1040045C) = (0x190 << 16) | 0xF0; //Screen Width and Height
	*((u32*)0x10400460) = 0x1c100d1;
	*((u32*)0x10400464) = 0x1920002;
	*((u32*)0x10400470) = 0x80340; //Frame Buffer Format
	*((u32*)0x1040049C) = 0;
	
	*((u32*)0x10400468) = 0x18300000; //Left Framebuffer 0
	*((u32*)0x1040046C) = 0x18300000; //Left Framebuffer 1
	*((u32*)0x10400494) = 0x18300000; //Right Framebuffer 0
	*((u32*)0x10400498) = 0x18300000; //Right Framebuffer 1
	*((u32*)0x10400478) = 1; //Framebuffer Select
	*((u32*)0x10400474) = 0x10501;
	*/
	
	//memset((void*)0x18300000, 0, 0x46500 + 0x38400); //Clear the framebuffer
}

void top_screen(void* top_buf){
	memcpy((void*)0x18300000, top_buf, 0x46500);
}

void bottom_screen(void* bottom_buf){
	memcpy((void*)0x18346500, bottom_buf, 0x38400);
}