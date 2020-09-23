	.file	"f24.cc"
	.text
	.globl	_Z1fPKci
	.type	_Z1fPKci, @function
_Z1fPKci:
.LFB30:
	endbr64
	pushq	%rbp
	pushq	%rbx
	subq	$8, %rsp
	movq	%rdi, %rbp
	movl	%esi, %ebx
	testl	%esi, %esi
	jle	.L2
.L3:
	movq	%rbp, %rdi
	call	puts@PLT
	subl	$1, %ebx
	jne	.L3
.L2:
	movl	$0, %eax
	addq	$8, %rsp
	popq	%rbx
	popq	%rbp
	ret
.LFE30:
	.size	_Z1fPKci, .-_Z1fPKci
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
