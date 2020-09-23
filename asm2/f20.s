	.file	"f20.cc"
	.text
	.globl	_Z1fPKc
	.type	_Z1fPKc, @function
_Z1fPKc:
.LFB30:
	endbr64
	subq	$8, %rsp
	call	puts@PLT
	addq	$8, %rsp
	ret
.LFE30:
	.size	_Z1fPKc, .-_Z1fPKc
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
