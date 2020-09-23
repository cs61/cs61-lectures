	.file	"fun04.cc"
	.text
	.globl	_Z3funPKc
	.type	_Z3funPKc, @function
_Z3funPKc:
.LFB0:
	endbr64
	movzbl	(%rdi), %eax
	testb	%al, %al
	je	.L4
	cmpb	1(%rdi), %al
	jne	.L4
	movsbl	2(%rdi), %eax
	ret
.L4:
	movl	$-1, %eax
	ret
.LFE0:
	.size	_Z3funPKc, .-_Z3funPKc
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
