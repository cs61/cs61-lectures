	.file	"fun15.cc"
	.text
	.globl	_Z3funPKc
	.type	_Z3funPKc, @function
_Z3funPKc:
.LFB0:
	endbr64
	movzbl	(%rdi), %eax
	testb	%al, %al
	jne	.L3
	jmp	.L4
.L8:
	addq	$1, %rdi
	testb	%al, %al
	je	.L4
.L3:
	movl	%eax, %edx
	movzbl	1(%rdi), %eax
	cmpb	%al, %dl
	jnb	.L8
	movl	$-1, %eax
	ret
.L4:
	xorl	%eax, %eax
	ret
.LFE0:
	.size	_Z3funPKc, .-_Z3funPKc
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
