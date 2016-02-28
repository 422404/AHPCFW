CC=arm-none-eabi-gcc
CP=arm-none-eabi-g++
OC=arm-none-eabi-objcopy
OD=arm-none-eabi-objdump
LD=arm-none-eabi-ld

all:
	$(CC) -g source/*.c source/fatfs/*.c -c -march=armv5te -O2 -ffast-math -std=c99
	$(CC) -g source/*.s source/fatfs/*.s -c -march=armv5te
	mkdir -p temp
	mv start.o temp/start.o
	$(CC) -T 3ds.ld temp/start.o *.o 
	$(OC) -O binary a.out ahpcfw.bin
	rm *.out *.o temp/start.o
	rmdir temp