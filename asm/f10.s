	.file	"f10.cc"
	.text
	.globl	_Z4sum1v
	.type	_Z4sum1v, @function
_Z4sum1v:
	endbr64
	movl	b1(%rip), %eax
	addl	a1(%rip), %eax
	ret
	.size	_Z4sum1v, .-_Z4sum1v
	.globl	_Z4sum2v
	.type	_Z4sum2v, @function
_Z4sum2v:
	endbr64
	movl	b2(%rip), %eax
	addl	a2(%rip), %eax
	ret
	.size	_Z4sum2v, .-_Z4sum2v
	.globl	_Z4sum3v
	.type	_Z4sum3v, @function
_Z4sum3v:
	endbr64
	movl	b3(%rip), %eax
	addl	a3(%rip), %eax
	ret
	.size	_Z4sum3v, .-_Z4sum3v
	.ident	"GCC: (Ubuntu 14.2.0-4ubuntu2~24.04) 14.2.0"
	.section	.note.GNU-stack,"",@progbits
