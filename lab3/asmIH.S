#include <machine/asm.h>

.data
.global _C_LABEL(kbd_ih_asm)
.global _asmScan
_asmScan:
.short

.text
_C_LABEL(kbd_ih_asm):
	inb $0x64, %al
	and $0x1, %al
	jz end
	xor %eax, %eax
	inb $0x60, %al
	mov %al, _asmScan
end:
	ret

