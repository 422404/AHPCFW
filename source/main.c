#include "fatfs/ff.h"
#include "keydata.h"
#include "firm.h"
#include "draw.h"
#include "hid.h"
#include "i2c.h"

void _start(void){
	keydata_init();
	*((u32*)0x10000020) = 0x340; //Undocumented CONFIG Register, Allows SDMMC Access
	
	FATFS sdmc;
	if (f_mount(&sdmc, "0:", 0) == FR_OK){
		
		if (HIDKeyStatus() & KEY_R){
			clear_framebuffers();
			screen_init(); //Now some menu or something should go here, idk
			
			FIL img; //I got bored of nothing being on the screen during this loop :p
			u32 * imgbr;
			if (f_open(&img, "image.bin", FA_READ | FA_OPEN_EXISTING) == FR_OK){
				f_read(&img, (void*)0x25000000, 0x46500, imgbr);
				f_close(&img);
				
				draw_top_screen((void*)0x25000000);
			}
			
			u32 key;
			u8 status;
			while(1){
				key = HIDKeyStatus();
				status = i2cHIDStatus();
				
				if (key & KEY_B) break;
				
				if (status & i2c_Shut){ //There's not really a reason for this, I just think it's cool
					screen_deinit();
					while(!(status & i2c_Open)){
						status = i2cHIDStatus();
						
						if (status & i2c_Power){
							i2cWriteRegister(I2C_DEV_MCU, 0x20, 1); //Power Off
							while(1);
						}
					}
					screen_init();
				}
				
				if (status & i2c_Power){
					i2cWriteRegister(I2C_DEV_MCU, 0x20, 1);
					while(1);
				}
				
				if (status & i2c_Home){
					i2cWriteRegister(I2C_DEV_MCU, 0x20, (1 << 2)); //Reboot
					while(1);
				}
			}
		}
		
		FIL handle;
		u32 * br;
		if (f_open(&handle, "firm.bin", FA_READ | FA_OPEN_EXISTING) == FR_OK){
			f_read(&handle, (void*)0x24000000, f_size(&handle), br);
			f_close(&handle);
			
			if (ARM9_decrypt((u32*)0x24000000) != 3) firmlaunch((u32*)0x24000000);
		}
		f_mount(NULL, "0:", 0);
	}
	
	i2cWriteRegister(I2C_DEV_MCU, 0x20, 1);
	while(1);
}