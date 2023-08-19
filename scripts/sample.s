	.text
	.file	"sample.ll"
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	movq	$.L.str.0, -24(%rsp)
	leaq	-24(%rsp), %rax
	movq	%rax, -8(%rsp)
	movl	$6, -12(%rsp)
	movl	$6, %eax
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.type	.L.str.0,@object                # @.str.0
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.0:
	.asciz	"1"
	.size	.L.str.0, 2

	.section	".note.GNU-stack","",@progbits
