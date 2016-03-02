.arm
.section .text.start

LDR R0, =0x101401C0         @ R0 = PDN_SPI_CNT;
LDR R0, [R0]                @ R0 = *(u32*)R0;
AND R0, #3                  @ R0 = R0 & 3;
CMP R0, #0                  @ if (!R0)
BEQ _start                  @ _start();

LDR SP, =0x27000000         @ Set the Stack Pointer

LDR R0, =0xFFFF001D         @ Unprotected Bootrom | 32KB
MCR P15, 0, R0, C6, C0, 0   @ Set MPU Region 0
LDR R0, =0x00000035         @ ITCM | 128MB
MCR P15, 0, R0, C6, C1, 0   @ Set MPU Region 1
LDR R0, =0x08000029         @ ARM9 Memory | 2MB
MCR P15, 0, R0, C6, C2, 0   @ Set MPU Region 2
LDR R0, =0x10000035         @ IO Registers | 128MB
MCR P15, 0, R0, C6, C3, 0   @ Set MPU Region 3
LDR R0, =0x18000035         @ Video RAM | 128MB
MCR P15, 0, R0, C6, C4, 0   @ Set MPU Region 4
LDR R0, =0x1FF00025         @ DSP Memory | 512KB
MCR P15, 0, R0, C6, C5, 0   @ Set MPU Region 5
LDR R0, =0x1FF80025         @ AXIWRAM | 512KB
MCR P15, 0, R0, C6, C6, 0   @ Set MPU Region 6
LDR R0, =0x20000037         @ FCRAM | 256MB
MCR P15, 0, R0, C6, C7, 0   @ Set MPU Region 7

LDR R0, =0x33333333         @ R/W Flags (0b011 = R/W for Kern/User)
MCR P15, 0, R0, C5, C0, 2   @ Set MPU Region Data Privs
MCR P15, 0, R0, C5, C0, 3   @ Set MPU Region Instruction Privs

MOV R0, #0x85               @ Access Flags (0b1 = Active) - Bits(7, 2, 0)
MCR P15, 0, R0, C2, C0, 0   @ Set MPU Regions as Data Cacheable
MCR P15, 0, R0, C2, C0, 1   @ Set MPU Regions as Instruction Cacheable
MCR P15, 0, R0, C3, C0, 0   @ Set MPU Regions as Write Bufferable

MOV R0, #0                  @ Clear R0
MCR P15, 0, R0, C7, C5, 0   @ Invalidate iCache
MCR P15, 0, R0, C7, C6, 0   @ Invalidate dCache
MCR P15, 0, R0, C7, C10, 4  @ Drain Write Buffer

MRC P15, 0, R0, C1, C0, 0   @ Read the Control Register
ORR R0, #0x1000             @ Enable iCache
ORR R0, #4                  @ Enable dCache
ORR R0, #1                  @ Enable MPU
MCR P15, 0, R0, C1, C0, 0   @ Set the Control Register

BL _start                   @ _start();
