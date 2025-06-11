	.text
	.def	@feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 0
	.file	"main"
	.def	main;
	.scl	2;
	.type	32;
	.endef
	.section	.rdata,"dr"
	.p2align	3, 0x0                          # -- Begin function main
.LCPI0_0:
	.quad	0x4000000000000000              # double 2
.LCPI0_1:
	.quad	0x3ff0000000000000              # double 1
	.text
	.globl	main
	.p2align	4, 0x90
main:                                   # @main
.seh_proc main
# %bb.0:
	pushq	%rbp
	.seh_pushreg %rbp
	subq	$48, %rsp
	.seh_stackalloc 48
	leaq	48(%rsp), %rbp
	.seh_setframe %rbp, 48
	.seh_endprologue
	callq	__main
	movq	$0, -8(%rbp)
	movb	$1, %al
	testb	%al, %al
	jne	.LBB0_3
# %bb.1:
	movsd	.LCPI0_1(%rip), %xmm0           # xmm0 = [1.0E+0,0.0E+0]
	jmp	.LBB0_2
.LBB0_3:                                # %ifcond
	movsd	.LCPI0_0(%rip), %xmm0           # xmm0 = [2.0E+0,0.0E+0]
.LBB0_2:
	addq	$48, %rsp
	popq	%rbp
	retq
	.seh_endproc
                                        # -- End function
