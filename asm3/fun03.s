	.file	"fun03.cc"
	.text
	.globl	_Z3funPKc
	.type	_Z3funPKc, @function
_Z3funPKc:
.LFB0:
	endbr64
	cmpb	$0, (%rdi)
	je	.L3
	movsbl	1(%rdi), %eax
	ret
.L3:
	movl	$-1, %eax
	ret
.LFE0:
	.size	_Z3funPKc, .-_Z3funPKc
	.ident	"GCC: (Ubuntu 10.2.0-5ubuntu2) 10.2.0"
	.section	.note.GNU-stack,"",@progbits
