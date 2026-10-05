CC      = riscv32-unknown-elf-gcc
OBJCOPY = riscv32-unknown-elf-objcopy

# Added 'm' for division/modulo instructions, and zicsr for CSR instructions
CFLAGS  = -march=rv32im_zicsr -mabi=ilp32 -ffreestanding -O2 -Wall
LDFLAGS = -nostdlib -T dtekv-script.lds

all: main.bin

boot.o: boot.S
	$(CC) $(CFLAGS) -c boot.S -o boot.o

testphoto.o: testphoto.c
	$(CC) $(CFLAGS) -c main.c -o main.o

# dtekv-script.lds automatically pulls in boot.o, so only pass testphoto.o here
main.elf: boot.o main.o
	$(CC) $(CFLAGS) $(LDFLAGS) main.o -o main.elf

main.bin: main.elf
	$(OBJCOPY) -O binary main.elf main.bin

clean:
	rm -f *.elf *.bin *.o

.PHONY: all clean
