#include "hid.h"
#include "types.h"

#include <strings.h>

int keypress(char* key){
	if (strncasecmp(key, "A", 1))		return *((u32*)0x10146000) & (1 << 0);
	if (strncasecmp(key, "B", 1))		return *((u32*)0x10146000) & (1 << 1);
	if (strncasecmp(key, "Select", 6))	return *((u32*)0x10146000) & (1 << 2);
	if (strncasecmp(key, "Start", 5))	return *((u32*)0x10146000) & (1 << 3);
	if (strncasecmp(key, "Right", 5))	return *((u32*)0x10146000) & (1 << 4);
	if (strncasecmp(key, "Left", 4))	return *((u32*)0x10146000) & (1 << 5);
	if (strncasecmp(key, "Up", 2))		return *((u32*)0x10146000) & (1 << 6);
	if (strncasecmp(key, "Down", 4))	return *((u32*)0x10146000) & (1 << 7);
	if (strncasecmp(key, "R", 1))		return *((u32*)0x10146000) & (1 << 8);
	if (strncasecmp(key, "L", 1))		return *((u32*)0x10146000) & (1 << 9);
	if (strncasecmp(key, "X", 1))		return *((u32*)0x10146000) & (1 << 10);
	if (strncasecmp(key, "Y", 1))		return *((u32*)0x10146000) & (1 << 11);
	return *((u32*)0x10146000);
}