	.file	"fun07.cc"
	.text
	.globl	_Z3funPKc
	.type	_Z3funPKc, @function
_Z3funPKc:
.LFB25:
	endbr64
	subq	$8, %rsp
	xorl	%edx, %edx
	xorl	%esi, %esi
	call	strtol@PLT
	cmpl	$2, %eax
	jle	.L2
	movl	%eax, %edi
	movl	$2, %esi
.L6:
	movl	%esi, %ecx
	imull	%esi, %ecx
	cmpl	%ecx, %edi
	jle	.L3
	leal	1(%rsi), %edx
	imull	%esi, %edx
.L4:
	movl	%edx, %ecx
	addl	%esi, %edx
	cmpl	%ecx, %edi
	jg	.L4
.L3:
	cmpl	%ecx, %edi
	je	.L7
	addl	$1, %esi
	cmpl	%esi, %edi
	jne	.L6
.L2:
	cmpl	$1, %eax
	setle	%al
	addq	$8, %rsp
	movzbl	%al, %eax
	ret
.L7:
	movl	$1, %eax
	addq	$8, %rsp
	ret
.LFE25:
	.size	_Z3funPKc, .-_Z3funPKc
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
