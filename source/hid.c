#include "hid.h"
#include "i2c.h"

u32 HIDKeyStatus(){
	return ~*((vu32*)0x10146000);
}

u8 i2cHIDStatus(){
	return i2cReadRegister(I2C_DEV_MCU, 0x10);
}
