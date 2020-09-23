	.file	"fun09.cc"
	.text
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"r"
	.text
	.globl	_Z3funPKc
	.type	_Z3funPKc, @function
_Z3funPKc:
.LFB30:
	endbr64
	subq	$8, %rsp
	leaq	.LC0(%rip), %rsi
	call	fopen@PLT
	testq	%rax, %rax
	je	.L3
	movq	%rax, %rdi
	call	fclose@PLT
	xorl	%eax, %eax
.L1:
	addq	$8, %rsp
	ret
.L3:
	movl	$-1, %eax
	jmp	.L1
.LFE30:
	.size	_Z3funPKc, .-_Z3funPKc
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
