	.file	"f18.cc"
	.text
	.globl	_Z9ptr_arithml
	.type	_Z9ptr_arithml, @function
_Z9ptr_arithml:
.LFB0:
	endbr64
	leaq	(%rdi,%rsi,4), %rax
	ret
.LFE0:
	.size	_Z9ptr_arithml, .-_Z9ptr_arithml
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
