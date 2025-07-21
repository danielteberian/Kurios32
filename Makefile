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
ASM_S = asm/boot.asm asm/gdt_asm.asm asm/switch.asm asm/syscalls_asm.asm asm/page_fault_handler_asm.asm asm/intr_handlers.asm
CPP_S = kernel.cpp sh.cpp kbd.cpp idt.cpp task.cpp gdt.cpp vfs.cpp testing/test.cpp testing/mem_test.cpp graphics/graphics.cpp paging.cpp syscalls.cpp usr/uswitch.cpp usr/uload.cpp error.cpp debug.cpp testing/vfs_test.cpp util/convert.cpp mem.cpp
OBJ = $(ASM_S:.asm=.o) $(CPP_S:.cpp=.o)
KERNEL = kurios32.bin



# Targets

all: $(KERNEL)

# Build filesystem
initrd.tar:
	mkdir -p initrd
	tar -cf initrd.tar -C initrd .

$(KERNEL): $(OBJ) initrd.tar
	$(CPP) -o $(KERNEL) $(OBJ) $(LD_F)

%.o: %.asm
	$(ASM) $(ASM_F) $< -o $@

%.o: %.cpp
	$(CPP) $(CPP_F) $< -o $@

run: all
	qemu-system-i386 -vga std -kernel $(KERNEL) -initrd initrd.tar

clean:
	rm -f $(OBJ) $(KERNEL)

clean_all:
	rm -f $(OBJ) $(KERNEL) initrd.tar
	rm -rf initrd

# Create a filesystem with any existing data
initrd.tar: $(wildcard initrd/*)
	mkdir -p initrd
	tar -cf initrd.tar -C initrd .


# Extract and rebuild initrd
extract_initrd:
	mkdir -p initrd
	tar -xf initrd.tar -C initrd 2>dev/null || true