	.file	"f12.cc"
	.text
	.globl	_Z4sum4v
	.type	_Z4sum4v, @function
_Z4sum4v:
	endbr64
	movzwl	a4(%rip), %eax
	movzwl	b4(%rip), %edx
	addl	%edx, %eax
	ret
	.size	_Z4sum4v, .-_Z4sum4v
	.globl	_Z4sum5v
	.type	_Z4sum5v, @function
_Z4sum5v:
	endbr64
	movswl	a5(%rip), %eax
	movswl	b5(%rip), %edx
	addl	%edx, %eax
	ret
	.size	_Z4sum5v, .-_Z4sum5v
	.globl	_Z4sum6v
	.type	_Z4sum6v, @function
_Z4sum6v:
	endbr64
	movzbl	a6(%rip), %eax
	movzbl	b6(%rip), %edx
	addl	%edx, %eax
	ret
	.size	_Z4sum6v, .-_Z4sum6v
	.globl	_Z4sum7v
	.type	_Z4sum7v, @function
_Z4sum7v:
	endbr64
	movq	b7(%rip), %rax
	addq	a7(%rip), %rax
	ret
	.size	_Z4sum7v, .-_Z4sum7v
	.globl	_Z4sum8v
	.type	_Z4sum8v, @function
_Z4sum8v:
	endbr64
	movq	b8(%rip), %rax
	addq	a8(%rip), %rax
	ret
	.size	_Z4sum8v, .-_Z4sum8v
	.globl	_Z4sum9v
	.type	_Z4sum9v, @function
_Z4sum9v:
	endbr64
	movl	b9(%rip), %eax
	addl	a9(%rip), %eax
	ret
	.size	_Z4sum9v, .-_Z4sum9v
	.ident	"GCC: (Ubuntu 14.2.0-4ubuntu2~24.04) 14.2.0"
	.section	.note.GNU-stack,"",@progbits
