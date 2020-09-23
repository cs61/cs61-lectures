	.file	"fun01.cc"
	.text
	.globl	_Z3funPKc
	.type	_Z3funPKc, @function
_Z3funPKc:
.LFB27:
	endbr64
	subq	$8, %rsp
	movl	$33, %esi
	call	strchr@PLT
	testq	%rax, %rax
	sete	%al
	addq	$8, %rsp
	movzbl	%al, %eax
	ret
.LFE27:
	.size	_Z3funPKc, .-_Z3funPKc
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
