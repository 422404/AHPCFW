#include "draw.h"

#include <string.h>

void screen_init(void){ //Does nothing for now, no need for the screen
	/*	The idea is to have arm11 control this */ 
	if (*((u32*)0x10141200) & 1 == 1){ //A9LH
		return;
	} else {
		return;
	}
}