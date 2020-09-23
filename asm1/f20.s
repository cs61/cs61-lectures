	.file	"f20.cc"
	.text
	.globl	_Z1fiii
	.type	_Z1fiii, @function
_Z1fiii:
.LFB0:
	endbr64
	imull	$1000, %edi, %edi
	imull	$-10000, %esi, %esi
	addl	%esi, %edi
	imull	$-100000, %edx, %eax
	addl	%edi, %eax
	ret
.LFE0:
	.size	_Z1fiii, .-_Z1fiii
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
