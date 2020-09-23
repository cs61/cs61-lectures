	.file	"f07.cc"
	.text
	.globl	_Z4sum1v
	.type	_Z4sum1v, @function
_Z4sum1v:
.LFB0:
	endbr64
	movl	b1(%rip), %eax
	addl	a1(%rip), %eax
	ret
.LFE0:
	.size	_Z4sum1v, .-_Z4sum1v
	.globl	_Z4sum2v
	.type	_Z4sum2v, @function
_Z4sum2v:
.LFB1:
	endbr64
	movl	b2(%rip), %eax
	addl	a2(%rip), %eax
	ret
.LFE1:
	.size	_Z4sum2v, .-_Z4sum2v
	.globl	_Z4sum3v
	.type	_Z4sum3v, @function
_Z4sum3v:
.LFB2:
	endbr64
	movl	b3(%rip), %eax
	addl	a3(%rip), %eax
	ret
.LFE2:
	.size	_Z4sum3v, .-_Z4sum3v
	.globl	_Z4sum4v
	.type	_Z4sum4v, @function
_Z4sum4v:
.LFB3:
	endbr64
	movzwl	a4(%rip), %eax
	movzwl	b4(%rip), %edx
	addl	%edx, %eax
	ret
.LFE3:
	.size	_Z4sum4v, .-_Z4sum4v
	.globl	_Z4sum5v
	.type	_Z4sum5v, @function
_Z4sum5v:
.LFB4:
	endbr64
	movzbl	a5(%rip), %eax
	movzbl	b5(%rip), %edx
	addl	%edx, %eax
	ret
.LFE4:
	.size	_Z4sum5v, .-_Z4sum5v
	.globl	_Z4sum6v
	.type	_Z4sum6v, @function
_Z4sum6v:
.LFB5:
	endbr64
	movq	b6(%rip), %rax
	addq	a6(%rip), %rax
	ret
.LFE5:
	.size	_Z4sum6v, .-_Z4sum6v
	.globl	_Z4sum7v
	.type	_Z4sum7v, @function
_Z4sum7v:
.LFB6:
	endbr64
	movq	b7(%rip), %rax
	addq	a7(%rip), %rax
	ret
.LFE6:
	.size	_Z4sum7v, .-_Z4sum7v
	.globl	_Z4sum8v
	.type	_Z4sum8v, @function
_Z4sum8v:
.LFB7:
	endbr64
	movl	b8(%rip), %eax
	addl	a8(%rip), %eax
	ret
.LFE7:
	.size	_Z4sum8v, .-_Z4sum8v
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
