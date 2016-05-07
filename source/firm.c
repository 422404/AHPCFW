#include "firm.h"
#include "patches.h"
#include "keydata.h"
#include "aes.h"
#include "fatfs/ff.h"
#include "fatfs/sdmmc.h"
#include "draw.h"
#include "hid.h"
#include "arm11.h"

#include <string.h>
#include <stdlib.h>

/* TODO:
	-Finish ARM11 Thread
*/

int REDNAND(void){
	u8 buf[0x200];
	sdmmc_sdcard_readsectors(1, 1, buf); //"rednand"
	if (strncmp((char *)(buf+0x100), "NCSD", 4) == 0){
		emunand_code[22] = 1;
		emunand_code[23] = 1;
		return 0;
	}
	
	mmcdevice *nand = getMMCDevice(0);
	sdmmc_sdcard_readsectors(nand->total_size, 1, buf); //"emunand"
	if (strncmp((char *)(buf+0x100), "NCSD", 4) == 0){
		emunand_code[22] = 0;
		emunand_code[23] = nand->total_size;
		return 0;
	}
	
	return -1;
}

int firm_setup(void){
	if (strncmp((char *)FIRM, "FIRM", 4) != 0) return -1; //if not firm
	
	u8* arm9bin = (void*)FIRM + FIRM[0xA0/4];
	
	if (arm9bin[0] != 0xA7 || arm9bin[1] != 0x38) return 0; //if o3ds firm
	
	if (arm9bin[0x800] != 0x70 || arm9bin[0x801] != 0x47){ //if encrypted
		if (arm9bin[0x50] != 0xFF && arm9bin[0x61] != 0xA9){ //if 9.6^ firm
			keydata_init(0x1B, 0);
			keydata_init(0x11, 1);
		}
		else keydata_init(0x11, 0);
		
		u8 keyslot = arm9bin[0x50] != 0xFF ? 0x16 : 0x15; //keyslot changed on 9.5
		u8* keyX = arm9bin + (arm9bin[0x50] != 0xFF ? 0x60 : 0);
		
		set_keyslot(0x11);
		aes(keyX, keyX, NULL, 1, AES_ECB_DECRYPT); //keyX is encrypted with aes ecb
		
		set_keyX(keyslot, keyX);
		set_keyY(keyslot, arm9bin+0x10); //keyY must be set last
		
		set_keyslot(keyslot);
		aes(arm9bin+0x800, arm9bin+0x800, arm9bin+0x20, atoi((const char *)(arm9bin+0x30))/0x10, AES_CTR_DECRYPT);
	}
	
	return 0;
}

void patch11(void){
	/* ARM11 */
	u32* arm11bin = (void*)FIRM + FIRM[0x70/4];
	u32 arm11size = FIRM[0x78/4];
	
	for (u32 i = 0; i < (arm11size/4); i++){
		/* SVC Access Check */
		if (arm11bin[i] == 0x0AFFFFEA){
			arm11bin[i] = 0xE320F000; //NOP
			arm11bin[i+2] = 0xE320F000; //NOP
		}
		
		/* ARM11 Thread (WIP) 
		if (arm11bin[i] == 0xE59F1028 && arm11bin[i+2] == 0xE59F0000){
			memcpy((void*)thread_addr, arm11_thread, sizeof(arm11_thread));
			
			arm11bin[i+2] = 0xE12FFF11; //BX R1
			arm11bin[i+12] = thread_addr; //R1
		}*/
	}
	
	/* ARM9 */
	u32* arm9bin = (void*)FIRM + FIRM[0xA0/4];
	u32 arm9size = FIRM[0xA8/4];
	
	/* FIRM Partition Update (Credit to Delebile) */
	if (!(*((vu32 *)0x101401C0) & 0x3)){ //Check for a9lh (Credit to AuroraWright)
		u8 FIRMUpdate[] = { 0x00, 0x28, 0x01, 0xDA, 0x04, 0x00 };
		void *address = memsearch((void*)arm9bin, arm9size, "exe:/%016llx/.firm", sizeof("exe:/%016llx/.firm"));
		if (address != NULL){
			address = memsearch((void*)(address - 0x100), 0x100, FIRMUpdate, 6);
			if (address != NULL){
				*((vu16 *)address) = 0x2000;
				*((vu16 *)(address+2)) = 0x46C0;
			}
		}
	}
	
	for (u32 i = 0; i < (arm9size/4); i++){
		/* MPU Setup */
		if (arm9bin[i] == 0xE1833618 && arm9bin[i+1] == 0xE1822007){ //MPU Permissions
			arm9bin[i] = 0xE59F20F4; //LDR R2, [PC, #0xF4]
			arm9bin[i+1] = 0xE59F30F0; //LDR R3, [PC, #0xF0]
		}
		
		if (arm9bin[i+1] == 0xEE060F10){ //MPU Regions
			arm9bin[i] = 0xE59F0098; //LDR R0, [PC, #0x98]
			arm9bin[i+2] = 0xE3A00035; //MOV R0, #0x00000035 ; ITCM/DTCM
			arm9bin[i+4] = 0xE59F00A0; //LDR R0, [PC, #0xA0]
			arm9bin[i+6] = 0xE59F00A4; //LDR R0, [PC, #0xA4]
			arm9bin[i+8] = 0xE59F00A8; //LDR R0, [PC, #0xA8]
			arm9bin[i+0xA] = 0xE59F00AC; //LDR R0, [PC, #0xAC]
			arm9bin[i+0xC] = 0xE59F00B0; //LDR R0, [PC, #0xB0]
			arm9bin[i+0xE] = 0xE59F00B4; //LDR R0, [PC, #0xB4]
		}
		
		if (arm9bin[i] == 0x01010101 && arm9bin[i+1] == 0x001C0505) //MPU Table
			memcpy((void*)arm9bin + (i*4), mpu_table, 0x60);
		
		if (arm9bin[i] == 0xEE052F50 && arm9bin[i+0x28] != 0x01010101){ //Don't update memory R/W permissions!
			arm9bin[i] = 0xE320F000; //NOP
			arm9bin[i+1] = 0xE320F000; //NOP
		}
		
		/* Signature Checks */
		if (arm9bin[i] == 0x4D22B570 && arm9bin[i+1] == 0x6869000C) arm9bin[i] = 0x47702000;
		if (arm9bin[i] == 0xE7761CC0) arm9bin[i] = 0xE7762000;
		
		/* FIRM Header Address */
		if (arm9bin[i] == 0x01FFFC00) arm9bin[i] = 0x24000000;
	}
}

void patch9(void){
	__asm__ volatile (
		"PUSH {R0}\n"
		
		/* Disable ITCM and DTCM */
		"MRC P15, 0, R0, C1, C0, 0\n"
		"BIC R0, #0x50000\n"
		"MCR P15, 0, R0, C1, C0, 0\n"
		
		/* Map ITCM */
		"MOV R0, #0x0000001E\n"
		"MCR P15, 0, R0, C9, C1, 1\n"
		
		/* Map DTCM */
		"LDR R0, =0x01FF800C\n"
		"MCR P15, 0, R0, C9, C1, 0\n"
		
		/* Enable ITCM and DTCM */
		"MRC P15, 0, R0, C1, C0, 0\n"
		"ORR R0, #0x50000\n"
		"MCR P15, 0, R0, C1, C0, 0\n"
		
		"POP {R0}"
	);
	
	/* Copy ITCM data to DTCM */
	memcpy((void*)0x01FFB700, (void*)0x00A0B700, 0x900);
	memcpy((void*)0x01FFC000, (void*)0x00A0C000, 0xDE4);
	memcpy((void*)0x01FFD000, (void*)0x00A0D000, 0x2470);
	
	/* Clear out ITCM */
	memset((void*)0x00A08000, 0, 0x8000);
	
	/* ARM9 */
	u32* arm9bin = (void*)FIRM + FIRM[0xA0/4];
	u32 arm9size = FIRM[0xA8/4];
	
	/* Red/EmuNAND */
	if (!(HIDKeyStatus() & KEY_L) && (REDNAND() == 0)){
		for (u32 i = 0; i < (arm9size/4); i++){
			/* SDMC Struct */
			if (arm9bin[i] == 0x30201820){
				emunand_code[21] = arm9bin[i+2] + arm9bin[i+3];
				memcpy((void*)0x00A0FFA0, emunand_code, 0x60);
			}
					
			/* Branch to ITCM */
			if (arm9bin[i] == 0x000D0004 && arm9bin[i+1] == 0x001E0017 && arm9bin[i+0x10] == 0x000D0004){
				arm9bin[i] = 0x47A04C00; //LDR R4, =0x00A0FFA0; BLX R4
				arm9bin[i+1] = 0x00A0FFA0;
				
				arm9bin[i+0x10] = 0x47A04C00;
				arm9bin[i+0x11] = 0x00A0FFA0;
				break;
			}
		}
	}
		
	/* ARM9 Thread */
	FIL thread;
	u32 thread_br = 0;
	if (f_open(&thread, "thread.bin", FA_READ | FA_OPEN_EXISTING) == FR_OK){
		if (f_size(&thread) <= 0x7F00){ //Max thread size
			for (u32 i = 0; i < (arm9size/4); i++){
				if (arm9bin[i] == 0xE59F002C && arm9bin[i+1] == 0xE59F102C){
					arm9_thread[13] = arm9bin[i+13]; //Set R0
					
					memcpy((void*)0x00A0FF60, arm9_thread, 0x38);
					f_read(&thread, (void*)0x00A08000, f_size(&thread), &thread_br);
					
					arm9bin[i+1] = 0xE12FFF30; //BLX R0
					arm9bin[i+13] = 0x00A0FF60;
					break;
				}
			}
		}
		f_close(&thread);
	}
}

void debug_dump(void){
	FIL dtcm;
	u32 dtcm_br = 0;
	f_open(&dtcm, "dtcm.bin", FA_WRITE | FA_CREATE_ALWAYS);
	f_write(&dtcm, (void*)0x01FF8000, 0x8000, &dtcm_br);
	f_close(&dtcm);
	
	FIL itcm;
	u32 itcm_br = 0;
	f_open(&itcm, "itcm.bin", FA_WRITE | FA_CREATE_ALWAYS);
	f_write(&itcm, (void*)0x00A08000, 0x8000, &itcm_br);
	f_close(&itcm);
	
	FIL arm9;
	u32 arm9_br = 0;
	f_open(&arm9, "arm9.bin", FA_WRITE | FA_CREATE_ALWAYS);
	f_write(&arm9, (void*)FIRM + FIRM[0xA0/4], FIRM[0xA8/4], &arm9_br);
	f_close(&arm9);
	
	FIL arm11;
	u32 arm11_br = 0;
	f_open(&arm11, "arm11.bin", FA_WRITE | FA_CREATE_ALWAYS);
	f_write(&arm11, (void*)FIRM + FIRM[0x70/4], FIRM[0x78/4], &arm11_br);
	f_close(&arm11);
}

void firmlaunch(void){
	if (firm_setup() == 0){
		debug(patch11); //Have ARM9 and ARM11 patch firm at the same time
		patch9();
		while(ARM11Entry); //Wait for ARM11 to finish (if it hasn't already)
		//debug_dump();
		
		memcpy((void*)FIRM[0x44/4], (void*)FIRM + FIRM[0x40/4], FIRM[0x48/4]);
		memcpy((void*)FIRM[0x74/4], (void*)FIRM + FIRM[0x70/4], FIRM[0x78/4]);
		memcpy((void*)FIRM[0xA4/4], (void*)FIRM + FIRM[0xA0/4], FIRM[0xA8/4]);
		
		ARM11(screen_deinit);
		ARM11Entry = FIRM[0x8/4];
		((void (*)())0x0801B01C)();
	}
}
