	.file	"f25.cc"
	.text
	.globl	_Z1fiii
	.type	_Z1fiii, @function
_Z1fiii:
	endbr64
	pushq	%rbp
	movq	%rsp, %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	-4(%rbp), %eax
	cmpl	-8(%rbp), %eax
	jl	.L2
	movl	-4(%rbp), %eax
	cmpl	-12(%rbp), %eax
	jl	.L2
	movl	-4(%rbp), %eax
	jmp	.L3
.L2:
	movl	-8(%rbp), %eax
	cmpl	-12(%rbp), %eax
	jl	.L4
	movl	-8(%rbp), %eax
	jmp	.L3
.L4:
	movl	-12(%rbp), %eax
.L3:
	popq	%rbp
	ret
	.size	_Z1fiii, .-_Z1fiii
	.ident	"GCC: (Ubuntu 14.2.0-4ubuntu2~24.04) 14.2.0"
	.section	.note.GNU-stack,"",@progbits
