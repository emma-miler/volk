	.text
	.file	"sample.ll"
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	subq	$24, %rsp
	.cfi_def_cfa_offset 32
	movq	$.L.str.0, 16(%rsp)
	movl	$.L.str.0, %edi
	callq	printf@PLT
	movl	$2, 12(%rsp)
	movl	$2, %eax
	addq	$24, %rsp
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.type	.L.str.0,@object                # @.str.0
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.0:
	.asciz	"test123\n"
	.size	.L.str.0, 9

	.section	".note.GNU-stack","",@progbits
