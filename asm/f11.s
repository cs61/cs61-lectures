	.file	"f11.cc"
	.text
	.globl	_Z3sumv
	.type	_Z3sumv, @function
_Z3sumv:
	endbr64
	movzwl	b(%rip), %eax
	addw	a(%rip), %ax
	ret
	.size	_Z3sumv, .-_Z3sumv
	.ident	"GCC: (Ubuntu 14.2.0-4ubuntu2~24.04) 14.2.0"
	.section	.note.GNU-stack,"",@progbits
