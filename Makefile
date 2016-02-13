CC=arm-none-eabi-gcc
CP=arm-none-eabi-g++
OC=arm-none-eabi-objcopy
OD=arm-none-eabi-objdump
LD=arm-none-eabi-ld

LIBDIRS := C:\devkitPro\devkitARM\arm-none-eabi C:\devkitPro\devkitARM C:\devkitPro\libctru C:\devkitPro\libnds
CFLAGS=-c -mcpu=arm946e-s -march=armv5te -mlittle-endian -fshort-wchar -std=c99
SFLAGS=-c -mcpu=arm946e-s -march=armv5te -mlittle-endian -fshort-wchar

all:
	$(CC) -g source/*.c source/fatfs/*.c $(CFLAGS)
	$(CC) -g source/*.s source/fatfs/*.s -I source $(SFLAGS)
	mkdir -p temp
	mv start.o temp/start.o
	$(CC) -nostdlib -T 3ds.ld temp/start.o *.o $(OBJS)
	$(OC) -O binary a.out ahpcfw.bin
	rm *.out *.o temp/start.o
	rmdir temp