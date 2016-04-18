#include "firm.h"
#include "keydata.h"
#include "aes.h"
#include "fatfs/ff.h"
#include "fatfs/sdmmc.h"
#include "draw.h"
#include "hid.h"
#include "arm11.h"

#include <string.h>
#include <stdlib.h>

int firm_setup(u32* FIRM){
	if (strncmp((char *)FIRM, "FIRM", 4) != 0) return -1; //if not firm
	
	u8* arm9bin = (void*)FIRM + FIRM[0xA0/4];
	
	if (arm9bin[0] != 0xA7 || arm9bin[1] != 0x38) return 0; //if o3ds firm
	
	if (arm9bin[0x800] != 0x70 || arm9bin[0x801] != 0x47){ //if encrypted
		if (arm9bin[0x50] != 0xFF && arm9bin[0x61] != 0xA9) keydata_init(0x11, 1, NULL); //if 9.6^ firm
		else keydata_init(0x11, 0, NULL);
		
		u8 keyslot = arm9bin[0x50] != 0xFF ? 0x16 : 0x15; //keyslot changed on 9.5
		u8* keyX = arm9bin + (arm9bin[0x50] != 0xFF ? 0x60 : 0);
		
		set_keyslot(0x11);
		aes(keyX, keyX, NULL, 1, AES_ECB_DECRYPT); //keyX is encrypted with aes ecb
		
		set_keyX(keyslot, keyX);
		set_keyY(keyslot, arm9bin+0x10); //keyY must be set last
		
		set_keyslot(keyslot);
		aes(arm9bin+0x800, arm9bin+0x800, arm9bin+0x20, atoi((const char *)(arm9bin+0x30))/16, AES_CTR_DECRYPT);
	}
	keydata_init(0x1B, 0, FIRM);
	
	memcpy((void*)FIRM[0x44/4], (void*)FIRM + FIRM[0x40/4], FIRM[0x48/4]);
	memcpy((void*)FIRM[0x74/4], (void*)FIRM + FIRM[0x70/4], FIRM[0x78/4]);
	memcpy((void*)FIRM[0xA4/4], (void*)FIRM + FIRM[0xA0/4], FIRM[0xA8/4]);
	
	return 0;
}

bool REDNAND(u32 offset, u32 ncsd_offset){
	u8 buff[0x200];
	
	if (strncmp((char *)(buff+0x100), "NCSD", 4) == 0){
		offset = 1;
		ncsd_offset = 1;
		return true;
	}
	
	mmcdevice nand = *getMMCDevice(0);
	if (strncmp((char *)(buff+0x100), "NCSD", 4) == 0){
		offset = 1;
		ncsd_offset = nand.total_size;
		return true;
	}
	
	return false;
}

void patch(u32* FIRM){
	/* ARM11 PATCHES */
	u32* arm11bin = (void*)FIRM[0x74/4];
	u32 arm11size = FIRM[0x78/4];
	
	/* SVC Access Check */
	for (u32 i = 0; i < (arm11size/4); i++){
		if (arm11bin[i] == 0x0AFFFFEA){
			arm11bin[i] = 0xE320F000;
			arm11bin[i+2] = 0xE320F000;
			break;
		}
	}
	
	/* Custom ARM11 Process (WIP) */ //https://gist.github.com/TiniVi/3d686676933d9b8f8bfe
	FIL cxi;
	u32 * cbr = 0;
	u32 process_size = FIRM[0x48/4];
	if (f_open(&cxi, "arm11.cxi", FA_READ | FA_OPEN_EXISTING) == FR_OK){
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
	}
	
	/* ARM9 PATCHES */
	u32* arm9bin = (void*)FIRM[0xA4/4];
	u32 arm9size = FIRM[0xA8/4];
	
	/* FIRM Partition Update (Credit to Delebile) */
	if (!(*((vu32 *)0x101401C0) & 0x3)){ //Check for a9lh (Credit to AuroraWright)
		u8 FIRMUpdate[] = { 0x00, 0x28, 0x01, 0xDA, 0x04, 0x00 };
		for (u32 i = 0; i < arm9size; i+=2){
			if (memcmp((void*)(arm9bin+i), "exe:/%016llx/.firm", 0x12) == 0){
				for (i -= 0x100; i < FIRM[0xA8/4]; i+=2){
					if (memcmp((void*)(arm9bin+i), &FIRMUpdate, 6) == 0){
						*((u16 *)(void*)(arm9bin+i)) = 0x2000;
						*((u16 *)(void*)(arm9bin+i+2)) = 0x46C0;
						break;
					}
				}
				break;
			}
		}
	}
	
	for (u32 i = 0; i < (arm9size/4); i++){
		/* MPU Flags */
		if (arm9bin[i] == 0x00240003){
			arm9bin[i] = 0x00360003;
			arm9bin[i+6] = 0x00200603;
			arm9bin[i+9] = 0x001C0603;
		}
		
		/* Signature Checks */
		if (arm9bin[i] == 0x4D22B570 && arm9bin[i+1] == 0x6869000C){
			arm9bin[i] = 0x47702000;
		}
		if (arm9bin[i] == 0xE7761CC0){
			arm9bin[i] = 0xE7762000;
		}
		
		/* FIRM Header Address */
		if (arm9bin[i] == 0x01FFFC00){
			arm9bin[i] = 0x24000000;
			break;
		}
	}
	
	u32 emunand_code[] = { //Credit to Normmatt
		0xE1A04000 /* MOV R4, R0 */, 
		0xE1A05001 /* MOV R5, R1 */, 
		0xE1A07002 /* MOV R7, R2 */, 
		0xE1A06003 /* MOV R6, R3 */, 
		0xE5902004 /* LDR R2, [R0, #4] */, 
		0xE59F1038 /* LDR R1, =sdmc_struct */, 
		0xE1520001 /* CMP R2, R1 */, 
		0x0A000007 /* BEQ locret */, 
		0xE5801004 /* STR R1, [R0, #4] */, 
		0xE5902008 /* LDR R2, [R0, #8] */, 
		0xE3520000 /* CMP R2, #0 */, 
		0xE59F3024 /* LDR R3, =nand_loc */, 
		0xE0822003 /* ADD R2, R3 */, 
		0x059F3020 /* LDREQ R3, =ncsd_loc */, 
		0x00822003 /* ADDEQ R2, R3 */, 
		0xE5802008 /* STR R2, [R0, #8] */, 
		/* locret: */ 
		0xE1A01005 /* MOV R1, R5 */, 
		0xE1A02007 /* MOV R2, R7 */, 
		0xE1A03006 /* MOV R3, R6 */, 
		0xE28EE004 /* ADD LR, #4 */, 
		0xE12FFF1E /* BX LR */, 
		0x00000000 /* sdmc_struct */, 
		0x00000000 /* nand_loc */, 
		0x00000000 /* ncsd_loc */ 
	};
	
	u32 offset = 0;
	u32 ncsd = 0;
		for (u32 i = 0; i < (arm9size/4); i++){
			if (arm9bin[i] == 0x30201820){
				emunand_code[21] = arm9bin[i+2] + arm9bin[i+3]; //sdmc struct
				emunand_code[22] = offset; //nand offset
				emunand_code[23] = ncsd; //ncsd offset
				memcpy((void*)0x01FFF470, emunand_code, sizeof(emunand_code));
			}
			
			if (arm9bin[i] == 0x000D0004 && arm9bin[i+1] == 0x001E0017 && arm9bin[i+0x10] == 0x000D0004){
				arm9bin[i] = 0x47A04C00; //LDR R4, =0x01FFF470; BLX R4
				arm9bin[i+1] = 0x01FFF470;
				
				arm9bin[i+0x10] = 0x47A04C00;
				arm9bin[i+0x11] = 0x01FFF470;
				break;
			}
		}
	}
	
	u32 thread_code[] = { 
		0xE3A0003F /* MOV R0, #0x3F */, 
		0xE59F101C /* LDR R1, =entry */, 
		0xE3A02000 /* MOV R2, #0 */, 
		0xE59F3018 /* LDR R3, =stack_top */, 
		0xE3A04001 /* MOV R4, #1 */, 
		0xEF000008 /* SVC 8 */, 
		0xE59F0000 /* LDR R0, =stored_r0 */, 
		0xE59FF000 /* LDR PC, =stored_lr */, 
		0x00000000 /* stored_r0 */, 
		0x00000000 /* stored_lr */, 
		0x01FF8034 /* entry */, 
		0x08000C00 /* stack_top */ 
	};
	
	FIL thread;
	u32 * tbr = 0;
	if (f_open(&thread, "thread.bin", FA_READ | FA_OPEN_EXISTING) == FR_OK){
		if (f_size(&thread) <= 0x36CC){ //Max thread size
			for (u32 i = 0; i < arm9size/4; i++){
				if (arm9bin[i] == 0xE59F002C && arm9bin[i+1] == 0xE59F102C){
					thread_code[8] = arm9bin[i+13]; //Set R0
					thread_code[9] = (u32)arm9bin + (i*4) + 4; //Set LR
					
					memcpy((void*)0x01FF8004, thread_code, sizeof(thread_code));
					f_read(&thread, (void*)0x01FF8034, f_size(&thread), tbr);
					
					arm9bin[i] = 0xE59FF02C;
					arm9bin[i+13] = 0x01FF8004;
					break;
				}
			}
		}
		f_close(&thread);
	}
}

void firmlaunch(u32* FIRM){
	if (firm_setup(FIRM) == 0){
		patch(FIRM);
		ARM11(screen_deinit);
		ARM11Entry = FIRM[0x8/4];
		((void (*)())0x0801B01C)(); //We don't need the ARM9Loader since we're preforming it's tasks
	}
}
