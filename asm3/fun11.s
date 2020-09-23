	.file	"fun11.cc"
	.text
	.globl	_Z3funPKc
	.type	_Z3funPKc, @function
_Z3funPKc:
.LFB0:
	endbr64
	movzbl	(%rdi), %eax
	subl	$65, %eax
	cmpb	$25, %al
	ja	.L4
	movl	$1, %eax
.L3:
	movzbl	(%rdi,%rax), %edx
	movl	%eax, %esi
	addq	$1, %rax
	leal	-65(%rdx), %ecx
	cmpb	$25, %cl
	jbe	.L3
	cmpl	$5, %esi
	setle	%al
	testb	%dl, %dl
	setne	%dl
	orl	%edx, %eax
	movzbl	%al, %eax
	ret
.L4:
	movl	$1, %eax
	ret
.LFE0:
	.size	_Z3funPKc, .-_Z3funPKc
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
