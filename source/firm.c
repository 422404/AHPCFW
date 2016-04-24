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

int firm_setup(void){
	if (strncmp((char *)FIRM, "FIRM", 4) != 0) return -1; //if not firm
	
	u8* arm9bin = (void*)FIRM + FIRM[0xA0/4];
	
	if (arm9bin[0] != 0xA7 || arm9bin[1] != 0x38) return 0; //if o3ds firm
	
	if (arm9bin[0x800] != 0x70 || arm9bin[0x801] != 0x47){ //if encrypted
		if (arm9bin[0x50] != 0xFF && arm9bin[0x61] != 0xA9) keydata_init(0x11, 1); //if 9.6^ firm
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
	keydata_init(0x1B, 0);
	
	return 0;
}

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

void patch(void){
	/* ARM11 PATCHES */
	u32* arm11bin = (void*)FIRM + FIRM[0x70/4];
	u32 arm11size = FIRM[0x78/4];
	
	/* SVC Access Check */
	for (u32 i = 0; i < (arm11size/4); i++){
		if (arm11bin[i] == 0x0AFFFFEA){
			arm11bin[i] = 0xE320F000;
			arm11bin[i+2] = 0xE320F000;
			break;
		}
	}
	
	/* ARM9 PATCHES */
	u32* arm9bin = (void*)FIRM + FIRM[0xA0/4];
	u32 arm9size = FIRM[0xA8/4];
	
	/* FIRM Partition Update (Credit to Delebile) */
	if (!(*((vu32 *)0x101401C0) & 0x3)){ //Check for a9lh (Credit to AuroraWright)
		u8 FIRMUpdate[] = { 0x00, 0x28, 0x01, 0xDA, 0x04, 0x00 };
		for (u32 i = 0; i < arm9size; i+=2){
			if (memcmp((void*)(arm9bin+i), "exe:/%016llx/.firm", 0x12) == 0){
				for (i -= 0x100; i < FIRM[0xA8/4]; i+=2){
					if (memcmp((void*)(arm9bin+i), &FIRMUpdate, 6) == 0){
						*((vu16 *)(void*)(arm9bin+i)) = 0x2000;
						*((vu16 *)(void*)(arm9bin+i+2)) = 0x46C0;
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
	
	/* ITCM Cleanup */
	memset((void*)0x01FF8000, 0, 0x3700);
	memset((void*)0x01FFCDE4, 0, 0x21C);
	memset((void*)0x01FFF470, 0, 0xB90);
	
	/* Red/EmuNAND */
	if (!(HIDKeyStatus() & KEY_L) && (REDNAND() == 0)){
		for (u32 i = 0; i < (arm9size/4); i++){
			/* SDMC Struct */
			if (arm9bin[i] == 0x30201820){
				emunand_code[21] = arm9bin[i+2] + arm9bin[i+3];
				memcpy((void*)0x01FFF470, emunand_code, sizeof(emunand_code));
			}
			
			/* Branch to ITCM */
			if (arm9bin[i] == 0x000D0004 && arm9bin[i+1] == 0x001E0017 && arm9bin[i+0x10] == 0x000D0004){
				arm9bin[i] = 0x47A04C00; //LDR R4, =0x01FFF470; BLX R4
				arm9bin[i+1] = 0x01FFF470;
				
				arm9bin[i+0x10] = 0x47A04C00;
				arm9bin[i+0x11] = 0x01FFF470;
				break;
			}
		}
	}
	
	/* Debugging 
	FIL itcm;
	u32 itcm_br = 0;
	f_open(&itcm, "itcm.bin", FA_WRITE | FA_CREATE_ALWAYS);
	f_write(&itcm, (void*)0x01FF8000, 0x8000, &itcm_br);
	f_close(&itcm);
	
	FIL arm9;
	u32 arm9_br = 0;
	f_open(&arm9, "arm9.bin", FA_WRITE | FA_CREATE_ALWAYS);
	f_write(&arm9, (void*)arm9bin, arm9size, &arm9_br);
	f_close(&arm9);*/
}

void firmlaunch(void){
	if (firm_setup() == 0){
		patch();
		memcpy((void*)FIRM[0x44/4], (void*)FIRM + FIRM[0x40/4], FIRM[0x48/4]);
		memcpy((void*)FIRM[0x74/4], (void*)FIRM + FIRM[0x70/4], FIRM[0x78/4]);
		memcpy((void*)FIRM[0xA4/4], (void*)FIRM + FIRM[0xA0/4], FIRM[0xA8/4]);
		ARM11(screen_deinit);
		ARM11Entry = FIRM[0x8/4];
		((void (*)())0x0801B01C)(); //We don't need the ARM9Loader since we're preforming it's tasks
	}
}
