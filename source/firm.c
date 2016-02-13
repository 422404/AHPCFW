#include "types.h"
#include "aes.h"
#include "standard.h"

int ARM9_decrypt(void* FIRM){ //Address of FIRM (Keyslot 0x11 needs to be set properly ahead of time)
	if(*((u8*)FIRM) != 'F' || *((u8*)FIRM + 1) != 'I' || *((u8*)FIRM + 2) != 'R' || *((u8*)FIRM + 3) != 'M') return 3; //if not firm
	
	u8* arm9bin = (void*)(FIRM + *((u32*)FIRM + 0xA0));
	
	if(arm9bin[0] != 0xA7 || arm9bin[1] != 0x38 || arm9bin[2] != 0x5F || arm9bin[3] != 0x46) return 2; //if o3ds firm
	
	if(arm9bin[0x61] != 0xA9 && arm9bin[0x50] != 0xFF){ //if 9.6 firm
		//set_normalKey(0x11, &AESKey2);
	} else {
		//set_normalKey(0x11, &AESKey1);
	}
	
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

/*
	TODO:
	Add ARM11 Thread Code
	Add NAND R/W Code
	Add ARM9 Thread Code
*/

void patch(void){
	u32* arm9bin = (void*)0x08006800;
	u32* axiwram = (void*)0x1FF80000;
	
	u32 SVCAccessCheck[] = { 0x0AFFFFEA, 0xE320F000 };
	for (int i = 0; i < 0x80000/4; i++){
		if (axiwram[i] == SVCAccessCheck[0]){
			axiwram[i] = SVCAccessCheck[1];
			axiwram[i+2] = SVCAccessCheck[1];
			break;
		}
	}
	
	u32 ARM11FreeSpace = 0;
	u32 ARM11KTimer[] = { 0x72656D69, 0 };
	for (int i = 0; i < 0x80000/4; i++){
		if ((axiwram[i] == ARM11KTimer[0]) && (axiwram[i+1] == ARM11KTimer[1])){
			ARM11FreeSpace = 0x1FF80000 + (i*4) + 0x8;
			break;
		}
	}
	
	/*
		Branch Instruction:
		0xEB = BL
		0xFF = branch_addr -= 0x40000
		0xXXXX - Distance from branch instruction divided by 4 (2 needs to be subtracted as it always branches by at least 8 bytes)
	*/
	
	u32 ARM11ThreadHook[] = { 0xF10C01C0, 0xE3A00000 }; //Disable Interrupts, MOV R0, #0
	for (int i = 0; i < 0x80000/4; i++){
		if ((axiwram[i] == ARM11ThreadHook[0]) && (axiwram[i+2] == ARM11ThreadHook[1])){
			axiwram[i] = 0xEBFF0000 | (((ARM11FreeSpace - (((i*4) + 0x1FF80000) - 0x40000))/4) - 2);
			break;
		}
	}
	
	u32 SigCheck0[] = { 0x4D22B570, 0x47702000 };
	for (int i = 0; i < 0xF9800/4; i++){
		if (arm9bin[i] == SigCheck0[0]){
			arm9bin[i] = SigCheck0[1];
			break;
		}
	}
	
	u32 SigCheck1[] = { 0xE7761CC0, 0xE7762000 };
	for (int i = 0; i < 0xF9800/4; i++){
		if (arm9bin[i] == SigCheck1[0]){
			arm9bin[i] = SigCheck1[1];
			break;
		}
	}
	
	u32 ARM9FreeSpace = 0;
	u32 ARM9KTimer[] = { 0x69544B00, 0x0072656D };
	for (int i = 0; i < 0xF9800/4; i++){
		if ((arm9bin[i] == ARM9KTimer[0]) && (arm9bin[i+1] == ARM9KTimer[1])){
			for (i += 0; i < 0xF9800/4; i++){
				if ((arm9bin[i] == 0) && (arm9bin[i+1] == 0xFFFFFFFF) && (arm9bin[i+2] == 0xFFFFFFFF)
				&& (arm9bin[i+3] == 0xFFFFFFFF) && (arm9bin[i+4] == 0xFFFFFFFF) && (arm9bin[i+5] == 0xFFFFFFFF)){
					ARM9FreeSpace = 0x08006800 + (i*4) + 0x10;
					break;
				}
			}
			break;
		}
	}
	
	u32 NANDRW[] = { 0x000D0004, 0x001E0017, 0x47204C00 };
	for (int i = 0; i < 0xF9800/4; i++){
		if ((arm9bin[i] == NANDRW[0]) && (arm9bin[i+1] == NANDRW[1]) && (arm9bin[i+0x10] == NANDRW[0])){
			arm9bin[i] = NANDRW[3];
			arm9bin[i+1] = ARM9FreeSpace;
			arm9bin[i+0x10] = NANDRW[3];
			arm9bin[i+0x11] = ARM9FreeSpace + 0xF0;
			break;
		}
	}
	
	/*
		Branch Instruction:
		0xEB = BL
		0xFE = branch_addr -= 0x80000
		0xXXXX - Distance from branch instruction divided by 4 (2 needs to be subtracted as it always branches by at least 8 bytes)
	*/
	
	u32 ARM9ThreadHook[] = { 0xE59F002C, 0xE59F102C };
	for (int i = 0; i < 0xF9800/4; i++){
		if ((arm9bin[i] == ARM9ThreadHook[0]) && (arm9bin[i+1] == ARM9ThreadHook[1])){
			arm9bin[i] = 0xEBFE0000 | ((((ARM9FreeSpace + 0x220) - (((i*4) + 0x08006800) - 0x80000))/4) - 2);
			break;
		}
	}
}

void firmlaunch(void* FIRM){
	_memcpy((void*)(FIRM + 0x44), (void*)(FIRM + *((u32*)FIRM + 0x40)), *((u32*)FIRM + 0x48));
	_memcpy((void*)(FIRM + 0x74), (void*)(FIRM + *((u32*)FIRM + 0x70)), *((u32*)FIRM + 0x48));
	_memcpy((void*)(FIRM + 0xA4), (void*)(FIRM + *((u32*)FIRM + 0xA0)), *((u32*)FIRM + 0x48));
	*((u32*)0x1FFFFFF8) = *((u32*)FIRM + 0x8);
	((void (*)())0x801B01C)(); //((void (*)())*((u32*)FIRM + 0xC))();
}
