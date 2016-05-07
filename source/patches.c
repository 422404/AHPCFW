#include "patches.h"

#include <string.h>

void *memsearch(void* start_address, u32 size, void* pattern, u32 pattern_size){
	for (int i = 0; i < (size - pattern_size); i++){
		if (memcmp(start_address+i, pattern, pattern_size) == 0)
			return start_address+i;
	}
	return NULL;
}

u32 arm11_thread[] = { 
	0xE12FFF31 /* BLX R1 */, 
	0xE92D5FFF /* PUSH {R0-R12, LR} */, 
	0xE3A0003F /* MOV R0, #0x3F */, 
	0xE59F1014 /* LDR R1, =entry */, 
	0xE3A02000 /* MOV R2, #0 */, 
	0xE59F3010 /* LDR R3, =stack_top */, 
	0xE3E04001 /* MOV R4, #-2 */, 
	0xEF000008 /* SVC 8 */, 
	0xE8BD5FFF /* POP {R0-R12, LR} */, 
	0xE12FFF1E /* BX LR */,
	0x00000000 /* entry */, 
	0x00000000 /* stack_top */, 
};

u32 mpu_table[] = { 
	0x01010101, 0x001C0505, 0xFFFF001D /* Unprotected Bootrom */, 
	0x00000001, 0x001C0603, 0x33333333 /* MPU Permissions */, 
	0x01010101, 0x00280603, 0x08000029 /* ARM9 Memory */, 
	0x00000001, 0x00370003, 0x10000035 /* IO */, 
	0x01000001, 0x00360303, 0x1800002D /* VRAM */, 
	0x01000001, 0x00360303, 0x1FF00025 /* DSP Memory */, 
	0x01010101, 0x00280603, 0x1FF80025 /* AXIWRAM */, 
	0x01000001, 0x00360303, 0x20000037 /* FCRAM */ 
};

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

u32 arm9_thread[] = { 
	0xE92D5FFF /* PUSH {R0-R12, LR} */,
	0xE3A0003F /* MOV R0, #0x3F */, 
	0xE59F101C /* LDR R1, =entry */, 
	0xE3A02000 /* MOV R2, #0 */, 
	0xE59F3018 /* LDR R3, =stack_top */, 
	0xE3E04001 /* MOV R4, #-2 */, 
	0xEF000008 /* SVC 8 */, 
	0xE8BD5FFF /* POP {R0-R12, LR} */,
	0xE59F000C /* LDR R0, =stored_r0 */, 
	0xE59E1030 /* LDR R1, [LR, #0x30] */, 
	0xE12FFF1E /* BX LR */,
	0x00A08000 /* entry */, 
	0x08000C00 /* stack_top */, 
	0x00000000 /* stored_r0 */
};