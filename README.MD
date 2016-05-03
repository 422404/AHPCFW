#Overview:
This is my take on a 'CFW'. Currently, it loads firm.bin from the SD Card, patches it, and launches it.
There are also screen functions as well as other libs the user can play with. I plan to add in more patches and uses.

At the current point in time, this is what it patches:

-SVC Access Checks

-FIRM Partition Writing (Credit to Delebile)

-Signature Checks

-FIRM Header Address

There is also support for:

-Red/EmuNAND.

-An ARM9 Thread ('thread.bin' can be loaded into memory @ 0x00A08000 with a max size of 0xFF00)

WIP:

-An ARM11 Thread

#Credits:
Laramie14 - Right hand man.

Normmatt - sdmmc, i2c libs, and Red/EmuNAND code.

ChaN - FATFS lib.

Delebile - FIRM Partition patch.

b1|1s, mid-kid, Normmatt, dark_samus, and everyone involved with screen init.

3DBrew and it's members - Keeping 3DBrew so full of information.

#A9LH Credits:
Seeing as Delebile's code is much more appealing, I decided there'd be no reason to share my implementation. Despite this,
the people who have helped me deserve to be acknowledged.

OctopusRift - Reviving the idea for a N3DS DG. Would've brushed it off otherwise.

Laramie14 - Right hand man, helped with key bruteforcing and verified that O3DS support was indeed possible.

Plutoo and Yellows8 - Arm9loaderhax's discovery and explanation, including the useful infos given at 32c3.

Normmatt - sdmmc libs, i2c libs, 1.0.0 CN QR code, and CN NAND dumper's 'load.bin'.

ChaN - FATFS lib.

3DBrew and it's members - Keeping 3DBrew so full of information.

#Shoutouts:
The following people either helped me or have been generally cool in one way or another (This list will probably expand):

Steveice

Apache Thunder

Hundshamer

MassExplosion

Motezazer

AlbertoSONIC

Julian20

TuxSH

Capito27

AuroraWright

MegaShaymin

RyanRocks462 (^.^)
