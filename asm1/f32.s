	.file	"f32.cc"
	.text
	.globl	_Z1fi
	.type	_Z1fi, @function
_Z1fi:
.LFB0:
	endbr64
	subq	$8, %rsp
	call	_Z1gi@PLT
	addq	$8, %rsp
	addl	$1, %eax
	ret
.LFE0:
	.size	_Z1fi, .-_Z1fi
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
