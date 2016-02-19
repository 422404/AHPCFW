.arm
.section .text.start

CPSID	AIF						@ Disable Interrupts
BL		_start					@ _start();
