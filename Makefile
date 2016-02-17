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
	$(CC) -T 3ds.ld temp/start.o *.o $(OBJS)
	$(OC) -O binary a.out ahpcfw.bin
	rm *.out *.o temp/start.o
	rmdir temp