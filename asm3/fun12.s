	.file	"fun12.cc"
	.text
	.globl	_Z3funPKc
	.type	_Z3funPKc, @function
_Z3funPKc:
.LFB27:
	endbr64
	pushq	%rbx
	movq	%rdi, %rbx
	call	strlen@PLT
	movzbl	(%rbx), %ecx
	movq	%rax, %rdi
	testb	%cl, %cl
	je	.L4
	cltq
	xorl	%edx, %edx
	leaq	-1(%rbx,%rax), %rsi
	jmp	.L3
.L8:
	movzbl	1(%rbx,%rdx), %ecx
	leal	1(%rdx), %r8d
	subq	$1, %rsi
	addq	$1, %rdx
	testb	%cl, %cl
	je	.L2
.L3:
	movl	%edx, %r8d
	cmpb	%cl, (%rsi)
	je	.L8
.L2:
	cmpl	$4, %edi
	popq	%rbx
	setle	%al
	cmpl	%r8d, %edi
	setne	%dl
	orl	%edx, %eax
	movzbl	%al, %eax
	ret
.L4:
	xorl	%r8d, %r8d
	jmp	.L2
.LFE27:
	.size	_Z3funPKc, .-_Z3funPKc
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
