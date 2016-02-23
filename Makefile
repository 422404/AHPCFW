CC=arm-none-eabi-gcc
CP=arm-none-eabi-g++
OC=arm-none-eabi-objcopy
OD=arm-none-eabi-objdump
LD=arm-none-eabi-ld

all:
	$(CC) -g source/*.c source/fatfs/*.c -c -mtune=arm946e-s -march=armv5te -DARM9 -O2 -ffast-math -fomit-frame-pointer -std=c99
	$(CC) -g source/*.s source/fatfs/*.s -c -mtune=arm946e-s -march=armv5te -DARM9
	mkdir -p temp
	mv start.o temp/start.o
	$(CC) -T 3ds.ld temp/start.o *.o -nostartfiles -mthumb -mthumb-interwork
	#-Wl,-Map,ahpcfw.map
	$(OC) -O binary a.out ahpcfw.bin
	rm *.out *.o temp/start.o
	rmdir temp