	.file	"f09.cc"
	.text
	.globl	_Z3sumv
	.type	_Z3sumv, @function
_Z3sumv:
.LFB0:
	endbr64
	movslq	b(%rip), %rax
	addq	a(%rip), %rax
	ret
.LFE0:
	.size	_Z3sumv, .-_Z3sumv
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
