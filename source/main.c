#include "fatfs/ff.h"
#include "keydata.h"
#include "firm.h"
#include "draw.h"
#include "hid.h"
#include "i2c.h"

#define FIRM (u32*)0x24000000

void MCU_ShutDown(void){
	i2cWriteRegister(I2C_DEV_MCU, 0x20, 1);
	while(1);
}

void MCU_Reboot(void){
	i2cWriteRegister(I2C_DEV_MCU, 0x20, 4);
	while(1);
}

void _start(void){
	keydata_init();
	*((vu32 *)0x10000020) = 0x340; //Undocumented CONFIG Register, Allows SDMMC Access
	
	FATFS sdmc;
	f_mount(&sdmc, "0:", 0);
	
	if (HIDKeyStatus() & KEY_R){
		clear_framebuffers();
		screen_init(); //Now some menu or something should go here, idk
		
		FIL img; //I got bored of nothing being on the screen during this loop :p
		u32 * ibr = 0;
		if (f_open(&img, "image.bin", FA_READ | FA_OPEN_EXISTING) == FR_OK){
			f_read(&img, TopDrawBuffer, 0x46500, ibr);
			f_close(&img);
			
			memcpy(BotDrawBuffer, TopDrawBuffer + ((0x46500-0x38400)/2), 0x38400); //copy the image to the bottom screen (centered)
			
			draw_top_screen();
			draw_bottom_screen();
		}
		
		u32 key;
		u8 i2c;
		while(1){
			key = HIDKeyStatus();
			i2c = HIDI2CStatus();
			
			if (key & KEY_B) break;
			
			if (i2c & i2c_Shut){ //There's not really a reason for this, I just think it's cool
				screen_deinit();
				while(!(i2c & i2c_Open)){
					i2c = HIDI2CStatus();
					if (i2c & i2c_Power) MCU_ShutDown();
				}
				screen_reinit();
			}
			
			if (i2c & i2c_Power) MCU_ShutDown();
			if (i2c & i2c_Home) MCU_Reboot();
		}
	}
	
	FIL handle;
	u32 * br = 0;
	if (f_open(&handle, "firm.bin", FA_READ | FA_OPEN_EXISTING) == FR_OK){
		f_read(&handle, FIRM, f_size(&handle), br);
		f_close(&handle);
		
		if (ARM9_decrypt(FIRM) != 3) firmlaunch(FIRM);
	}
	
	MCU_ShutDown();
}