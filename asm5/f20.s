	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 10, 15	sdk_version 10, 15, 6
	.globl	__Z1f2s1iS_             ## -- Begin function _Z1f2s1iS_
__Z1f2s1iS_:                            ## @_Z1f2s1iS_
## %bb.0:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	%rdi, %rax
	shrq	$32, %r8
	movq	$0, (%rdi)
	movq	$0, 16(%rdi)
	movq	$0, 8(%rdi)
	addl	%ecx, %esi
	addl	%r8d, %esi
	movl	%esi, 4(%rdi)
	popq	%rbp
	retq
                                        ## -- End function
.subsections_via_symbols
