	.file	"f33.cc"
	.text
	.globl	_Z1f5small
	.type	_Z1f5small, @function
_Z1f5small:
.LFB55:
	endbr64
	movl	%edi, %eax
	movsbl	%ah, %edx
	movsbl	%dil, %eax
	movsbl	%dl, %edx
	leal	(%rax,%rdx,2), %eax
	ret
.LFE55:
	.size	_Z1f5small, .-_Z1f5small
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"0"
.LC1:
	.string	"%d.%d = %d\n"
	.section	.text.startup,"ax",@progbits
	.globl	main
	.type	main, @function
main:
.LFB56:
	endbr64
	pushq	%r12
	pushq	%rbp
	pushq	%rbx
	cmpl	$1, %edi
	jle	.L4
	movl	%edi, %ebp
	movq	8(%rsi), %rdi
	movq	%rsi, %r12
	xorl	%edx, %edx
	xorl	%esi, %esi
	call	strtol@PLT
	leaq	.LC0(%rip), %rdi
	movl	%eax, %ebx
	cmpl	$2, %ebp
	je	.L5
	movq	16(%r12), %rdi
.L5:
	xorl	%edx, %edx
	xorl	%esi, %esi
	call	strtol@PLT
	movq	%rax, %rcx
	xorl	%eax, %eax
	movb	%bl, %al
	movb	%cl, %ah
	movl	%eax, %edi
	call	_Z1f5small
	movsbl	%bl, %edx
	movl	$1, %edi
	leaq	.LC1(%rip), %rsi
	movl	%eax, %r8d
	movsbl	%cl, %ecx
	xorl	%eax, %eax
	call	__printf_chk@PLT
	popq	%rbx
	xorl	%eax, %eax
	popq	%rbp
	popq	%r12
	ret
.L4:
	leaq	.LC0(%rip), %rdi
	xorl	%edx, %edx
	xorl	%esi, %esi
	call	strtol@PLT
	leaq	.LC0(%rip), %rdi
	movl	%eax, %ebx
	jmp	.L5
.LFE56:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
