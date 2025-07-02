; This has the assembly code that is used by the kernel to load GDT and TSS

global flush_gdt
global flush_tss

flush_gdt:
	; Get the pointer to the GDT struct
	mov eax, [esp+4]
	; Load GDT pointer
	lgdt [eax]

	; Reload segment registers in order to use the GDT
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	; SUPER JUMP, reload the CS register
	jmp 0x08:.flush

.flush:
	ret

flush_tss:
	mov ax, 0x2B
	ltr ax
	ret
