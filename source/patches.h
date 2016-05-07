#ifndef PATCHES_H
#define PATCHES_H

#include "types.h"

void *memsearch(void* start_address, u32 size, void* pattern, u32 pattern_size);

extern u32 arm11_thread[];
extern u32 mpu_table[];
extern u32 emunand_code[];
extern u32 arm9_thread[];

#endif