	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 10, 15	sdk_version 10, 15, 6
	.globl	__Z1fllllll             ## -- Begin function _Z1fllllll
__Z1fllllll:                            ## @_Z1fllllll
## %bb.0:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movl	%edi, -32(%rbp)
	movl	%esi, -28(%rbp)
	movl	%edx, -24(%rbp)
	movl	%ecx, -20(%rbp)
	movl	%r8d, -16(%rbp)
	movl	%r9d, -12(%rbp)
	leaq	-8(%rbp), %rsi
	leaq	-32(%rbp), %rdi
	callq	__Z13process_arrayPKiS0_
	addq	$32, %rsp
	popq	%rbp
	retq
                                        ## -- End function
.subsections_via_symbols
