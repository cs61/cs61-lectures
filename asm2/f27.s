	.file	"f27.cc"
	.text
	.globl	_Z1fjj
	.type	_Z1fjj, @function
_Z1fjj:
.LFB0:
	endbr64
	cmpl	%esi, %edi
	movl	%esi, %eax
	cmovnb	%edi, %eax
	ret
.LFE0:
	.size	_Z1fjj, .-_Z1fjj
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
