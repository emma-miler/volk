	.text
	.file	"sample.ll"
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	movl	$5, -4(%rsp)
	movl	$16, -8(%rsp)
	movl	$5, -16(%rsp)
	movl	$16, -12(%rsp)
	movl	$1, -20(%rsp)
	movl	$2, -24(%rsp)
	movl	$3, -28(%rsp)
	movl	$10, -32(%rsp)
	movl	$160, %eax
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.section	".note.GNU-stack","",@progbits
