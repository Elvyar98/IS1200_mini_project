CC      = riscv32-unknown-elf-gcc
OBJCOPY = riscv32-unknown-elf-objcopy

# Dynamiska variabler för tester. Om inget anges används main och -O2.
OPT     ?= -O2
SRC     ?= main

# m for division/modulo instructions, and zicsr for CSR instructions
CFLAGS  = -march=rv32im_zicsr -mabi=ilp32 -ffreestanding $(OPT) -Wall
LDFLAGS = -nostdlib -T dtekv-script.lds

all: $(SRC).bin

boot.o: boot.S
	$(CC) $(CFLAGS) -c boot.S -o boot.o

$(SRC).o: $(SRC).c
	$(CC) $(CFLAGS) -c $(SRC).c -o $(SRC).o

# Länkar in alla beroenden ($^) automatiskt.
$(SRC).elf: boot.o $(SRC).o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(SRC).bin: $(SRC).elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f *.elf *.bin *.o

.PHONY: all clean