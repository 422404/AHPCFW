#include "aes.h"
#include "fatfs/ff.h"
#include "hid.h"
#include "draw.h"
#include "keydata.h"
#include "firm.h"

void _start(void){
	screen_init();
	keydata_init();
	*((u32*)0x10000020) = 0x340; //Undocumented CONFIG Register, Allows SDMMC Access
	
	FATFS fs;
	FIL firm;
	u32 br;
	if (f_mount(&fs, "0:", 0) == FR_OK){
		if (f_open(&firm, "firm.bin", FA_READ | FA_OPEN_EXISTING) == FR_OK){
			f_read(&firm, (void*)0x24000000, f_size(&firm), br);
			f_close(&firm);
			
			firmlaunch();
		}
	}
	f_mount(NULL, "0:", 0);
	
	while(1);
}