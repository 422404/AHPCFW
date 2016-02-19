#ifndef KEYDATA_H
#define KEYDATA_H

#include "types.h"

extern u8 keyX[0x10];
extern u8 AESKey1[0x10];
extern u8 AESKey2[0x10];
extern u8 keyXBuf[0x10];

void keydata_init(void);

#endif