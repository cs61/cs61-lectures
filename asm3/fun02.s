	.file	"fun02.cc"
	.text
	.globl	_Z3funPKc
	.type	_Z3funPKc, @function
_Z3funPKc:
.LFB52:
	endbr64
	subq	$8, %rsp
	xorl	%edx, %edx
	xorl	%esi, %esi
	call	strtol@PLT
	addq	$8, %rsp
	addl	$1, %eax
	ret
.LFE52:
	.size	_Z3funPKc, .-_Z3funPKc
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
