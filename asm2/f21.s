	.file	"f21.cc"
	.text
	.globl	_Z1fPKc
	.type	_Z1fPKc, @function
_Z1fPKc:
.LFB30:
	endbr64
	pushq	%rbx
	movq	%rdi, %rbx
.L2:
	movq	%rbx, %rdi
	call	puts@PLT
	jmp	.L2
.LFE30:
	.size	_Z1fPKc, .-_Z1fPKc
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
