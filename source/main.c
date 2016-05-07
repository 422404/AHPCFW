#include "fatfs/ff.h"
#include "keydata.h"
#include "arm11.h"
#include "firm.h"
#include "draw.h"
#include "hid.h"
#include "i2c.h"

void MCU_ShutDown(void){
	i2cWriteRegister(I2C_DEV_MCU, 0x20, 1);
	while(1);
}

void MCU_Reboot(void){
	i2cWriteRegister(I2C_DEV_MCU, 0x20, 4);
	while(1);
}

void main_loop(void){ //This is currently just here in case of accidental launch or something, I use it to verify the code launched
	screen_init(); //Now some menu or something should go here, idk
	
	update_top_screen();
	update_bottom_screen();
	
	while(1){
		if (HIDKeyStatus() & KEY_B) break;
		if (HIDI2CStatus() & i2c_Power) MCU_ShutDown();
		if (HIDI2CStatus() & i2c_Home) MCU_Reboot();
	}
}

void _start(void){
	keydata_init(0x25, 0);
	*((vu32 *)0x10000020) = 0x340; //Undocumented CONFIG Register, Allows SD/MMC Access
	
	FATFS sdmc;
	f_mount(&sdmc, "0:", 0);
	
	ARM11Start();
	if (HIDKeyStatus() & KEY_R){
		clear_framebuffers();
		
		FIL img; //I got bored of nothing being on the screen during the main loop :p
		u32 img_br = 0;
		if (f_open(&img, "image.bin", FA_READ | FA_OPEN_EXISTING) == FR_OK){
			f_read(&img, TopDrawBuffer, 0x46500, &img_br);
			f_close(&img);
			
			memcpy(BotDrawBuffer, TopDrawBuffer + 0x7080, 0x38400); //copy the image to the bottom screen (centered)
		}
		
		ARM11(main_loop);
	}
	
	FIL handle;
	u32 br = 0;
	if (f_open(&handle, "firm.bin", FA_READ | FA_OPEN_EXISTING) == FR_OK){
		f_read(&handle, (void*)FIRM, f_size(&handle), &br);
		f_close(&handle);
		
		firmlaunch();
	}
	
	MCU_ShutDown(); //Don't return!
}