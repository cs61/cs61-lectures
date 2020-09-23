	.file	"fun14.cc"
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
	pushq	%r15
	pushq	%r14
	pushq	%r13
	leaq	.LC0(%rip), %r13
	pushq	%r12
	xorl	%r12d, %r12d
	pushq	%rbp
	xorl	%ebp, %ebp
	pushq	%rbx
	movq	%rdi, %rbx
	subq	$24, %rsp
	movq	%fs:40, %rax
	movq	%rax, 8(%rsp)
	xorl	%eax, %eax
	leaq	4(%rsp), %r15
	movq	%rsp, %r14
	jmp	.L3
.L8:
	movl	(%rsp), %eax
	addl	%eax, %r12d
	testl	%eax, %eax
	setne	%al
	orl	%eax, %ebp
	movslq	4(%rsp), %rax
	addq	%rax, %rbx
.L3:
	xorl	%eax, %eax
	movq	%r15, %rcx
	movq	%r14, %rdx
	movq	%r13, %rsi
	movq	%rbx, %rdi
	call	__isoc99_sscanf@PLT
	cmpl	$1, %eax
	je	.L8
	testb	%bpl, %bpl
	movl	$-1, %eax
	cmove	%eax, %r12d
	movq	8(%rsp), %rax
	xorq	%fs:40, %rax
	jne	.L9
	addq	$24, %rsp
	movl	%r12d, %eax
	popq	%rbx
	popq	%rbp
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	ret
.L9:
	call	__stack_chk_fail@PLT
.LFE30:
	.size	_Z3funPKc, .-_Z3funPKc
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
