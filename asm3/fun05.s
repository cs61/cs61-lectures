	.file	"fun05.cc"
	.text
	.globl	_Z3funPKc
	.type	_Z3funPKc, @function
_Z3funPKc:
.LFB0:
	endbr64
	movzbl	(%rdi), %edx
	testb	%dl, %dl
	je	.L5
	movsbl	1(%rdi), %eax
	cmpb	%al, %dl
	jne	.L1
.L3:
	addq	$1, %rdi
	movl	%eax, %edx
	movsbl	1(%rdi), %eax
	cmpb	%dl, %al
	je	.L3
	ret
.L5:
	movl	$-1, %eax
.L1:
	ret
.LFE0:
	.size	_Z3funPKc, .-_Z3funPKc
	.ident	"GCC: (Ubuntu 10.2.0-5ubuntu2) 10.2.0"
	.section	.note.GNU-stack,"",@progbits
