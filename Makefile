CC=arm-none-eabi-gcc
CP=arm-none-eabi-g++
OC=arm-none-eabi-objcopy
OD=arm-none-eabi-objdump
LD=arm-none-eabi-ld

all:
	$(CC) -g source/*.c source/fatfs/*.c -c -mcpu=arm946e-s -march=armv5te -mlittle-endian -std=c99
	$(CC) -g source/*.s source/fatfs/*.s -I source -c -mcpu=arm946e-s -march=armv5te -mlittle-endian
	mkdir -p temp
	mv start.o temp/start.o
	$(CC) -T arm9.ld temp/start.o *.o
	$(OC) -O binary a.out ahpcfw.bin
	rm *.out *.o temp/start.o
	rmdir temp
	
	@echo
	
	$(CC) -g source/arm11/*.c -c -fPIE -fno-zero-initialized-in-bss -std=c99 -mcpu=mpcore
	$(CC) -g source/arm11/*.s -I arm11 -c -nostartfiles -nostdlib
	mkdir -p temp
	mv start.o temp/start.o
	$(LD) -T arm11.ld temp/start.o *.o
	$(OC) -O binary a.out debug.bin
	rm *.out *.o temp/start.o
	rmdir temp