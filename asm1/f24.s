	.file	"f24.cc"
	.text
	.globl	_Z1fllll
	.type	_Z1fllll, @function
_Z1fllll:
.LFB0:
	endbr64
	subq	%rsi, %rdi
	leaq	(%rdi,%rcx), %rax
	ret
.LFE0:
	.size	_Z1fllll, .-_Z1fllll
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
