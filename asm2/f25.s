	.file	"f25.cc"
	.text
	.globl	_Z1fiii
	.type	_Z1fiii, @function
_Z1fiii:
.LFB0:
	endbr64
	cmpl	%edx, %esi
	je	.L3
	movl	%esi, %eax
	ret
.L3:
	movl	%edi, %eax
	ret
.LFE0:
	.size	_Z1fiii, .-_Z1fiii
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
