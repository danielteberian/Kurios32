# Makefile for the kernel

# Build Tools
ASM = nasm
CPP = ./toolchain/bin/i386-elf-g++
LD = ./toolchain/i386-elf/bin/ld

# Flags
ASM_F = -f elf32
CPP_F = -I. -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -c
LD_F = -T linker.ld -nostdlib -lgcc

# Files to include
ASM_S = boot.asm gdt_asm.asm switch.asm
CPP_S = kernel.cpp sh.cpp kbd.cpp idt.cpp task.cpp gdt.cpp
OBJ = $(ASM_S:.asm=.o) $(CPP_S:.cpp=.o)
KERNEL = kurios32.bin



# Targets

all: $(KERNEL)

$(KERNEL): $(OBJ)
	$(CPP) -o $(KERNEL) $(OBJ) $(LD_F)

%.o: %.asm
	$(ASM) $(ASM_F) $< -o $@

%.o: %.cpp
	$(CPP) $(CPP_F) $< -o $@

run: all
	qemu-system-i386 -kernel $(KERNEL)

clean:
	rm -f $(OBJ) $(KERNEL)
