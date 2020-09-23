	.file	"fun13.cc"
	.text
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	" %d %n"
	.text
	.globl	_Z3funPKc
	.type	_Z3funPKc, @function
_Z3funPKc:
.LFB30:
	endbr64
	pushq	%r14
	pushq	%r13
	pushq	%r12
	xorl	%r12d, %r12d
	pushq	%rbp
	leaq	.LC0(%rip), %rbp
	pushq	%rbx
	movq	%rdi, %rbx
	subq	$16, %rsp
	movq	%fs:40, %rax
	movq	%rax, 8(%rsp)
	xorl	%eax, %eax
	leaq	4(%rsp), %r14
	movq	%rsp, %r13
	jmp	.L3
.L7:
	movslq	4(%rsp), %rax
	addl	(%rsp), %r12d
	addq	%rax, %rbx
.L3:
	xorl	%eax, %eax
	movq	%r14, %rcx
	movq	%r13, %rdx
	movq	%rbp, %rsi
	movq	%rbx, %rdi
	call	__isoc99_sscanf@PLT
	cmpl	$1, %eax
	je	.L7
	movq	8(%rsp), %rax
	xorq	%fs:40, %rax
	jne	.L8
	addq	$16, %rsp
	movl	%r12d, %eax
	popq	%rbx
	popq	%rbp
	popq	%r12
	popq	%r13
	popq	%r14
	ret
.L8:
	call	__stack_chk_fail@PLT
.LFE30:
	.size	_Z3funPKc, .-_Z3funPKc
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
