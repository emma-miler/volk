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
	movl	$42, -16(%rsp)
	movl	$16, -20(%rsp)
	movl	$6720, -12(%rsp)                # imm = 0x1A40
	movl	$6720, %eax                     # imm = 0x1A40
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.section	".note.GNU-stack","",@progbits
