#include "aes.h"
#include "fatfs/ff.h"
#include "hid.h"
#include "draw.h"
#include "keydata.h"
#include "firm.h"

void _start(void){
	keydata_init();
	*((u32*)0x10000020) = 0x340; //Undocumented CONFIG Register, Allows SDMMC Access
	
	FATFS fs;
	FIL firm;
	u32 * br;
	if (f_mount(&fs, "0:", 0) == FR_OK){
		if (f_open(&firm, "firm.bin", FA_READ | FA_OPEN_EXISTING) == FR_OK){
			f_read(&firm, (void*)0x24000000, f_size(&firm), br);
			f_close(&firm);
			
			if (ARM9_decrypt((void*)0x24000000) != 3){
				firmlaunch();
			}
		}
		
		FIL arm11; //debugging
		u32 * br11;
		if (f_open(&arm11, "debug.bin", FA_READ | FA_OPEN_EXISTING) == FR_OK){
			f_read(&arm11, (void*)0x1FFF4D00, f_size(&arm11), br11);
			f_close(&arm11);
			
			*((u32*)0x1FFFFFF8) = 0x1FFF4D00;
			screen_init();
		}
	}
	f_mount(NULL, "0:", 0);
	
	while(1);
}