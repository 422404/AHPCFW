#include "types.h"
#include "standard.h"

void _memcpy(u8* out, u8* in, int size){
    for (int i = 0; i < size; i++) out[i] = in[i];
}

int atoi(char* a_int){
	int ret = 0;
	for (int i = 0; a_int[i] != '\0'; i++) ret = ret*10 + a_int[i] - '0';
	return ret;
}