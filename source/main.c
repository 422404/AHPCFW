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
			screen_init();
			u32 key;
			u8 status;
			while(1){
				key = HIDKeyStatus();
				status = i2cHIDStatus();
				
				if (status & i2c_Shut){
					screen_deinit();
					while(!(i2cHIDStatus() & i2c_Open));
					screen_init();
				}
				if (status & i2c_Power){
					i2cWriteRegister(I2C_DEV_MCU, 0x20, 1);
					while(1);
				}
				if (status & i2c_Home) break;
			}
		}
		
		FIL firm;
		u32 * br;
		if (f_open(&firm, "firm.bin", FA_READ | FA_OPEN_EXISTING) == FR_OK){
			f_read(&firm, (void*)0x24000000, f_size(&firm), br);
			f_close(&firm);
			
			if (ARM9_decrypt((void*)0x24000000) != 3){
				screen_deinit();
				firmlaunch();
			}
		}
		f_mount(NULL, "0:", 0);
	}
	
	i2cWriteRegister(I2C_DEV_MCU, 0x20, 1);
	while(1);
}