#include "firm.h"
#include "keydata.h"
#include "aes.h"

#include <string.h>
#include <stdlib.h>

int ARM9_decrypt(void* FIRM){ //Address of FIRM (Keyslot 0x11 needs to be set properly ahead of time)
	if(*((u32*)FIRM) != 0x4D524946) return 3; //if not firm
	
	u8* arm9bin = (void*)(FIRM + *((u32*)FIRM + 0xA0));
	
	if(*((u32*)&arm9bin) != 0x465F38A7) return 2; //if o3ds firm
	if(*((u32*)&arm9bin+0x800) != 0x47704770) return 1; //if decrypted
	
	if(arm9bin[0x61] != 0xA9 && arm9bin[0x50] != 0xFF) set_normalKey(0x11, &AESKey2); //if 9.6 firm
	else set_normalKey(0x11, &AESKey1);
	
	int size = atoi(arm9bin + 0x30); //arm9bin encrypted data size
	u8* ctr = arm9bin + 0x20;
	
	int keyslot = arm9bin[0x61] == 0xA9 ? 0x16 : 0x15; //keyslot changed on 9.5
	int keyXAddr = arm9bin[0x61] == 0xA9 ? 0x60 : 0; //keyX Addr changed on 9.5
	
	u8* keyX = arm9bin + keyXAddr;
	
	set_keyslot(0x11);
	aes(&keyX, &keyX, NULL, 1, AES_ECB_DECRYPT); //keyX is encrypted with aes ecb
	
	set_keyX(keyslot, &keyX);
	set_keyY(keyslot, &arm9bin + 0x10); //keyY must be set last
	
	set_keyslot(keyslot);
	aes(&arm9bin + 0x800, &arm9bin + 0x800, ctr, size/16, AES_CTR_DECRYPT);
	
	return 0;
}

void patch(void){
	u32* FIRM = (void*)0x24000000;
	
	/* ARM11 PATCHES */
	u32* arm11bin = (void*)FIRM[0x74/4];
	
	/* SVC Access Check */
	for (u32 i = 0; i < (FIRM[0x78/4]/4); i++){
		if (arm11bin[i] == 0x0AFFFFEA){
			arm11bin[i] = 0xE320F000;
			arm11bin[i+2] = 0xE320F000;
			break;
		}
	}
	
	/*u32 ARM11FreeSpace = 0;
	for (u32 i = 0; i < (FIRM[0x78/4]/4); i++){
		if (arm11bin[i] == 0x72656D69 && arm11bin[i+1] == 0){ //"KTimer" (Probably gotta relocate)
			ARM11FreeSpace = FIRM[0x74/4] + (i*4) + 0x8;
			break;
		}
	}
	
		Branch Instruction:
		0xEB = BL
		0xFF = branch_addr -= 0x40000
		0xXXXX - Distance from branch instruction divided by 4 (2 needs to be subtracted as it always branches by at least 8 bytes)
	
	for (u32 i = 0; i < (FIRM[0x78/4]/4); i++){
		if (arm11bin[i] == 0xF10C01C0 && arm11bin[i+2] == 0xE3A00000){ //CPSID AIF; MOV R0, #0 (Probably gotta relocate)
			arm11bin[i+2] = 0xEBFF0000 | (((ARM11FreeSpace - (((i*4) + 0x1FF80000) - 0x40000))/4) - 2);
			break;
		}
	}*/
	
	/* ARM9 PATCHES */
	u32* arm9bin = (void*)FIRM[0xA4/4];
	
	/* Signature Check */
	for (u32 i = 0; i < (FIRM[0xA8/4]/4); i++){
		if (arm9bin[i] == 0x4D22B570 && arm9bin[i+1] == 0x6869000C){
			arm9bin[i] = 0x47702000;
		}
		if (arm9bin[i] == 0xE7761CC0){
			arm9bin[i] = 0xE7762000;
			break;
		}
	}
	
	/*u32 ARM9FreeSpace = 0;
	for (u32 i = 0; i < (FIRM[0xA8/4]/4); i++){
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
	
	for (u32 i = 0; i < (FIRM[0xA8/4]/4); i++){
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
	for (u32 i = 0; i < 0xF9800/4; i++){
		if (arm9bin[i] == 0xE59F002C && arm9bin[i+1] == 0xE59F102C){
			arm9bin[i] = 0xEBFE0000 | ((((ARM9FreeSpace + 0x220) - (((i*4) + 0x08006800) - 0x80000))/4) - 2);
			break;
		}
	}*/
}

void firmlaunch(void){
	u32* FIRM = (void*)0x24000000;
	memcpy((void*)FIRM[0x44/4], (void*)(0x24000000 + FIRM[0x40/4]), FIRM[0x48/4]);
	memcpy((void*)FIRM[0x74/4], (void*)(0x24000000 + FIRM[0x70/4]), FIRM[0x78/4]);
	memcpy((void*)FIRM[0xA4/4], (void*)(0x24000000 + FIRM[0xA0/4]), FIRM[0xA8/4]);
	patch();
	*((u32*)0x1FFFFFF8) = FIRM[0x8/4];
	((void (*)())0x801B01C)(); //((void (*)())FIRM[0xC/4])();
}
