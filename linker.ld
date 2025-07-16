ENTRY(start)

VIRTUAL_BASE = 0xC0000000;
PHYSICAL_BASE = 0x00100000;

SECTIONS {
	. = VIRTUAL_BASE + PHYSICAL_BASE;

	.text ALIGN(4K) : AT(ADDR(.text) - VIRTUAL_BASE) {
		*(.multiboot)
		*(.text)
	}

	.data ALIGN(4K) : AT(ADDR(.data) - VIRTUAL_BASE) {
		*(.data)
	}

	.bss ALIGN(4K) : AT(ADDR(.bss) - VIRTUAL_BASE) {
		*(.bss)
	}

	end = .;
}
