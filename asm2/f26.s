	.file	"f26.cc"
	.text
	.globl	_Z1fii
	.type	_Z1fii, @function
_Z1fii:
.LFB0:
	endbr64
	cmpl	%esi, %edi
	movl	%esi, %eax
	cmovge	%edi, %eax
	ret
.LFE0:
	.size	_Z1fii, .-_Z1fii
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
