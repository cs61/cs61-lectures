	.file	"f23.cc"
	.text
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"(null)"
	.text
	.globl	_Z1fPKc
	.type	_Z1fPKc, @function
_Z1fPKc:
.LFB30:
	endbr64
	subq	$8, %rsp
	testq	%rdi, %rdi
	je	.L2
	call	puts@PLT
.L3:
	movl	$0, %eax
	addq	$8, %rsp
	ret
.L2:
	leaq	.LC0(%rip), %rdi
	call	puts@PLT
	jmp	.L3
.LFE30:
	.size	_Z1fPKc, .-_Z1fPKc
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
