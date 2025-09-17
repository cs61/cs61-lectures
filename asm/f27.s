	.file	"f27.cc"
	.text
	.globl	_Z1fjjj
	.type	_Z1fjjj, @function
_Z1fjjj:
	endbr64
	cmpl	%edx, %esi
	cmovb	%edx, %esi
	cmpl	%edi, %esi
	movl	%edi, %eax
	cmovnb	%esi, %eax
	ret
	.size	_Z1fjjj, .-_Z1fjjj
	.ident	"GCC: (Ubuntu 13.2.0-23ubuntu4) 13.2.0"
	.section	.note.GNU-stack,"",@progbits
