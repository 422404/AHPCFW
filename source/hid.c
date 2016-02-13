#include "hid.h"
#include "standard.h"

int keypress(char* key){
	if (key[0] == 'A' || 'a') return *((u32*)0x10146000) & (1 << 0);
	if (key[0] == 'B' || 'b') return *((u32*)0x10146000) & (1 << 1);
	if ((key[0] == 'S' || 's') && (key[1] == 'E' || 'e')) return *((u32*)0x10146000) & (1 << 2);
	if ((key[0] == 'S' || 's') && (key[0] == 'T' || 't')) return *((u32*)0x10146000) & (1 << 3);
	if ((key[0] == 'R' || 'r') && (key[1] == 'I' || 'i')) return *((u32*)0x10146000) & (1 << 4);
	if ((key[0] == 'L' || 'l') && (key[1] == 'E' || 'e')) return *((u32*)0x10146000) & (1 << 5);
	if (key[0] == 'U' || 'u') return *((u32*)0x10146000) & (1 << 6);
	if (key[0] == 'D' || 'd') return *((u32*)0x10146000) & (1 << 7);
	if (key[0] == 'R' || 'r') return *((u32*)0x10146000) & (1 << 8);
	if (key[0] == 'L' || 'l') return *((u32*)0x10146000) & (1 << 9);
	if (key[0] == 'X' || 'x') return *((u32*)0x10146000) & (1 << 10);
	if (key[0] == 'Y' || 'y') return *((u32*)0x10146000) & (1 << 11);
	return 0;
}