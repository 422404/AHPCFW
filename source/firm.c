#include "firm.h"
#include "keydata.h"
#include "aes.h"
#include "fatfs/ff.h"
#include "draw.h"
#include "arm11.h"

#include <string.h>
#include <stdlib.h>

int firm_setup(u32* FIRM){
	if (FIRM[0] != 0x4D524946) return 1; //if not firm
	
	u8* arm9bin = (u8*)FIRM + FIRM[0xA0/4];
	
	if (*((u32*)arm9bin) != 0x465F38A7) return 0; //if o3ds firm
	
	keydata_init(0x1B, 0, FIRM);
	if (*((u32*)arm9bin+0x800) != 0x47704770) return 0; //if decrypted
	
	if (arm9bin[0x61] != 0xA9 && arm9bin[0x50] != 0xFF) keydata_init(0x11, 1, NULL); //if 9.6^ firm
	else keydata_init(0x11, 0, NULL);
	
	u8 keyslot = arm9bin[0x61] == 0xA9 ? 0x16 : 0x15; //keyslot changed on 9.5
	u8* keyX = arm9bin + (arm9bin[0x61] == 0xA9 ? 0x60 : 0);
	
	set_keyslot(0x11);
	aes(keyX, keyX, NULL, 1, AES_ECB_DECRYPT); //keyX is encrypted with aes ecb
	
	set_keyX(keyslot, keyX);
	set_keyY(keyslot, arm9bin + 0x10); //keyY must be set last
	
	set_keyslot(keyslot);
	aes(arm9bin + 0x800, arm9bin + 0x800, arm9bin + 0x20, atoi((const char *)(arm9bin + 0x30))/0x10, AES_CTR_DECRYPT);
	
	return 0;
}

void patch(u32* FIRM){
	/* ARM11 PATCHES */
	u32* arm11bin = FIRM + FIRM[0x70/4];
	u32 arm11size = FIRM[0x78/4];
	
	/* SVC Access Check */
	for (u32 i = 0; i < (arm11size/4); i++){
		if (arm11bin[i] == 0x0AFFFFEA){
			arm11bin[i] = 0xE320F000;
			arm11bin[i+2] = 0xE320F000;
			break;
		}
	}
	
	/* Custom ARM11 Process (WIP) //https://gist.github.com/TiniVi/3d686676933d9b8f8bfe
	FIL cxi;
	u32 * cbr = 0;
	u32 process_size = FIRM[0x48/4];
	if (f_open(&cxi, "arm11proc.bin", FA_READ | FA_OPEN_EXISTING) == FR_OK){
		f_read(&cxi, (void*)(FIRM[0x44/4] + process_size), f_size(&cxi), cbr);
		
		for (u32 i = 0; i < (arm11size/4); i++){
			if (arm11bin[i] == 0xE3570005){
				arm11bin[i] = 0xE3570006;
			}
			if (arm11bin[i] == 0xDFF00000 + process_size - 4){
				arm11bin[i] += f_size(&cxi);
			}
			if (arm11bin[i] == process_size){
				arm11bin[i] += f_size(&cxi);
				break;
			}
		}
		f_close(&cxi);
	}*/
	
	/* Enable Dev Stuff? */
	//*((vu8 *)0x1FF80014) &= ~1; //UNITINFO: Clear Production Bit
	//*((vu8 *)0x1FF80014) |= 2; //UNITINFO: Debug Bit
	//*((vu8 *)0x1FF80015) |= 1; //This is supposed to clear the Production Bit
	//*((vu8 *)0x1FF81004) = 2; //RUNNING_HW: Devboard
	
	/* ARM9 PATCHES */
	u32* arm9bin = FIRM + FIRM[0xA0/4];
	u32 arm9size = FIRM[0xA8/4];
	
	/* FIRM Partition Update (Credit to Delebile) */
	if (!(*((vu32 *)0x101401C0) & 0x3)){ //Check for a9lh (Credit to AuroraWright)
		u8 FIRMUpdate[] = { 0x00, 0x28, 0x01, 0xDA, 0x04, 0x00 };
		for (u32 i = 0; i < arm9size; i+=2){
			if (memcmp(arm9bin+i, "exe:/%016llx/.firm", 0x12) == 0){
				for (i -= 0x100; i < FIRM[0xA8/4]; i+=2){
					if (memcmp(arm9bin+i, &FIRMUpdate, 6) == 0){
						*((u16*)arm9bin+i) = 0x2000;
						*((u16*)arm9bin+i+2) = 0x46C0;
						break;
					}
				}
				break;
			}
		}
	}
	
	/* Signature Check */
	for (u32 i = 0; i < (arm9size/4); i++){
		if (arm9bin[i] == 0x4D22B570 && arm9bin[i+1] == 0x6869000C){
			arm9bin[i] = 0x47702000;
		}
		if (arm9bin[i] == 0xE7761CC0){
			arm9bin[i] = 0xE7762000;
			break;
		}
	}
	
	/*u32 ARM9FreeSpace = 0;
	for (u32 i = 0; i < (arm9size/4); i++){
		if (arm9bin[i] == 0x69544B00 && arm9bin[i+1] == 0x0072656D){ //"KTimer"
			for (i += 0; i < (FIRM[0xA8/4]/4); i++){
				if (arm9bin[i] == 0 && arm9bin[i+1] == 0xFFFFFFFF && arm9bin[i+2] == 0xFFFFFFFF
				&& arm9bin[i+3] == 0xFFFFFFFF && arm9bin[i+4] == 0xFFFFFFFF && arm9bin[i+5] == 0xFFFFFFFF){
					ARM9FreeSpace = FIRM[0xA4/4] + (i*4) + 0x10;
					break;
				}
			}
			break;
		}
	}
	
	u32 SDMMCStruct = 0;
	for (u32 i = 0; i < (arm9size/4); i++){
		if (arm9bin[i] == 0x30201820){
			SDMMCStruct = arm9bin[i+2] + arm9bin[i+3]; //0x11F0
			break;
		}
	}
	
	for (u32 i = 0; i < (arm9size/4); i++){
		if (arm9bin[i] == 0x000D0004 && arm9bin[i+1] == 0x001E0017 && arm9bin[i+0x10] == 0x000D0004){
			arm9bin[i] = 0x47204C00;
			arm9bin[i+1] = ARM9FreeSpace;
			arm9bin[i+0x10] = 0x47204C00;
			arm9bin[i+0x11] = ARM9FreeSpace + 0xF0;
			break;
		}
	}
	
		Branch Instruction:
		0xEB = BL
		0xFE = branch_addr -= 0x80000
		0xXXXX - Distance from branch instruction divided by 4 (2 needs to be subtracted as it always branches by at least 8 bytes)
	
	u32 ARM9ThreadHook[] = { 0xE59F002C, 0xE59F102C };
	for (u32 i = 0; i < arm9size/4; i++){
		if (arm9bin[i] == 0xE59F002C && arm9bin[i+1] == 0xE59F102C){
			arm9bin[i] = 0xEBFE0000 | ((((ARM9FreeSpace + 0x220) - (((i*4) + 0x08006800) - 0x80000))/4) - 2);
			break;
		}
	}*/
}

void firmlaunch(u32* FIRM){
	if (firm_setup(FIRM) == 0){
		patch(FIRM);
		ARM11(screen_deinit);
		memcpy((void*)FIRM[0x44/4], (void*)FIRM + FIRM[0x40/4], FIRM[0x48/4]);
		memcpy((void*)FIRM[0x74/4], (void*)FIRM + FIRM[0x70/4], FIRM[0x78/4]);
		memcpy((void*)FIRM[0xA4/4], (void*)FIRM + FIRM[0xA0/4], FIRM[0xA8/4]);
		ARM11Entry = FIRM[0x8/4];
		((void (*)())0x0801B01C)(); //We don't need the ARM9Loader since we're preforming it's tasks
	}
}
