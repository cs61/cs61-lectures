	.file	"f20.cc"
	.text
	.globl	_Z1fPKc
	.type	_Z1fPKc, @function
_Z1fPKc:
	endbr64
	pushq	%rbx
	movq	%rdi, %rbx
.L2:
	movq	%rbx, %rdi
	call	puts@PLT
	jmp	.L2
	.size	_Z1fPKc, .-_Z1fPKc
	.ident	"GCC: (Ubuntu 13.2.0-23ubuntu4) 13.2.0"
	.section	.note.GNU-stack,"",@progbits
