	.file	"f17.cc"
	.text
	.globl	_Z9ptr_arithPil
	.type	_Z9ptr_arithPil, @function
_Z9ptr_arithPil:
.LFB0:
	endbr64
	leaq	(%rdi,%rsi,4), %rax
	ret
.LFE0:
	.size	_Z9ptr_arithPil, .-_Z9ptr_arithPil
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
