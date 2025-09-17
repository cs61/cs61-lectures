	.file	"f15.cc"
	.text
	.globl	_Z12array_accessPil
	.type	_Z12array_accessPil, @function
_Z12array_accessPil:
	endbr64
	movl	(%rdi,%rsi,4), %eax
	ret
	.size	_Z12array_accessPil, .-_Z12array_accessPil
	.ident	"GCC: (Ubuntu 14.2.0-4ubuntu2~24.04) 14.2.0"
	.section	.note.GNU-stack,"",@progbits
