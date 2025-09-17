	.file	"f29.cc"
	.text
	.globl	_Z1fiii
	.type	_Z1fiii, @function
_Z1fiii:
	endbr64
	imull	$1000, %edi, %edi
	imull	%edx, %esi
	leal	(%rdi,%rsi), %eax
	ret
	.size	_Z1fiii, .-_Z1fiii
	.ident	"GCC: (Ubuntu 14.2.0-4ubuntu2~24.04) 14.2.0"
	.section	.note.GNU-stack,"",@progbits
