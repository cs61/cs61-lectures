	.file	"f31.cc"
	.text
	.globl	_Z1fRSt6vectorIiSaIiEE
	.type	_Z1fRSt6vectorIiSaIiEE, @function
_Z1fRSt6vectorIiSaIiEE:
.LFB900:
	endbr64
	movq	(%rdi), %rax
	movq	8(%rdi), %rsi
	cmpq	%rsi, %rax
	je	.L4
	movl	$0, %edx
.L3:
	movslq	(%rax), %rcx
	addq	%rcx, %rdx
	addq	$4, %rax
	cmpq	%rax, %rsi
	jne	.L3
.L1:
	movq	%rdx, %rax
	ret
.L4:
	movl	$0, %edx
	jmp	.L1
.LFE900:
	.size	_Z1fRSt6vectorIiSaIiEE, .-_Z1fRSt6vectorIiSaIiEE
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
