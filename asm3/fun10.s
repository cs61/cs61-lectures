	.file	"fun10.cc"
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
	movl	$1, %r8d
	movl	%eax, %ecx
	shrl	$8, %ecx
	je	.L1
	leal	-1(%rax), %edx
	xorl	%r8d, %r8d
	testl	%eax, %edx
	setne	%r8b
.L1:
	movl	%r8d, %eax
	addq	$8, %rsp
	ret
.LFE25:
	.size	_Z3funPKc, .-_Z3funPKc
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
