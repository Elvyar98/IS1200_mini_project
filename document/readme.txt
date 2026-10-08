README
Laser Speed Gate

1. File things
========================================
1. Download and unpack the zip file with the code.
2. Create the main folder in your home directory:
   mkdir ~/main
3. Move the source files main.c and boot.S, your Makefile, and the linker script dtekv-script.lds to the ~/main folder.

2. Compile
========================================
1. Navigate to the project folder in the terminal:
   cd ~/main
2. Build the program using the command:
   make

3. Connect DTEKV
========================================
1. Connect the DTEKV board to your computer using the USB cable.
2. Start JTAG:
   jtagd --user-start
3. Check that it detects the board:
   jtagconfig

4. Upload and Run the Program

5. Connect laser board to Powerbank and line it up with DTEK-V board.
=======================================
option 1: Lab Run Script
   ~/dtekv-tools/dtekv-run ~/main/main.bin

last reserve option: Manual Upload
   dtekv-upload main.bin 0x00000000

Library Path
======================================
earlier fault was solved by updating the librarypath.
   export LD_LIBRARY_PATH=$HOME/intelFPGA/23.1std/qprogrammer/linux64:$LD_LIBRARY_PATH