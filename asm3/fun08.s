	.file	"fun08.cc"
	.text
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"%d %d %d"
	.text
	.globl	_Z3funPKc
	.type	_Z3funPKc, @function
_Z3funPKc:
.LFB30:
	endbr64
	subq	$40, %rsp
	leaq	.LC0(%rip), %rsi
	movq	%fs:40, %rax
	movq	%rax, 24(%rsp)
	xorl	%eax, %eax
	leaq	16(%rsp), %rcx
	leaq	12(%rsp), %rdx
	leaq	20(%rsp), %r8
	call	__isoc99_sscanf@PLT
	cmpl	$3, %eax
	jne	.L4
	movl	16(%rsp), %eax
	addl	12(%rsp), %eax
	subl	20(%rsp), %eax
.L1:
	movq	24(%rsp), %rsi
	xorq	%fs:40, %rsi
	jne	.L7
	addq	$40, %rsp
	ret
.L4:
	movl	$-1, %eax
	jmp	.L1
.L7:
	call	__stack_chk_fail@PLT
.LFE30:
	.size	_Z3funPKc, .-_Z3funPKc
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
