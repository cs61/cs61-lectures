	.file	"fun06.cc"
	.text
	.globl	_Z3funPKc
	.type	_Z3funPKc, @function
_Z3funPKc:
.LFB27:
	endbr64
	xorl	%eax, %eax
	cmpb	$0, (%rdi)
	je	.L4
.L3:
	leal	1(%rax), %edx
	cmpb	$0, (%rdi,%rdx)
	movq	%rdx, %rax
	jne	.L3
	andl	$3, %eax
	ret
.L4:
	ret
.LFE27:
	.size	_Z3funPKc, .-_Z3funPKc
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
