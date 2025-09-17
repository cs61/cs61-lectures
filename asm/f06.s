	.file	"f06.cc"
	.text
	.globl	_Z1fPi
	.type	_Z1fPi, @function
_Z1fPi:
	endbr64
	movl	(%rdi), %eax
	ret
	.size	_Z1fPi, .-_Z1fPi
	.ident	"GCC: (Ubuntu 14.2.0-4ubuntu2~24.04) 14.2.0"
	.section	.note.GNU-stack,"",@progbits
