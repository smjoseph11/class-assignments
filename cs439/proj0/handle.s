	.file	"handle.c"
	.globl	STDOUT
	.section	.rodata
	.align 4
	.type	STDOUT, @object
	.size	STDOUT, 4
STDOUT:
	.long	1
.LC0:
	.string	"Nice try.\n"
.LC1:
	.string	"exiting\n"
	.text
	.globl	intr_handler
	.type	intr_handler, @function
intr_handler:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -20(%rbp)
	cmpl	$2, -20(%rbp)
	jne	.L2
	movl	STDOUT(%rip), %eax
	movl	$10, %edx
	movl	$.LC0, %esi
	movl	%eax, %edi
	call	write
	movq	%rax, -16(%rbp)
	cmpq	$10, -16(%rbp)
	je	.L1
	movl	$-999, %edi
	call	exit
.L2:
	cmpl	$10, -20(%rbp)
	jne	.L1
	movl	STDOUT(%rip), %eax
	movl	$8, %edx
	movl	$.LC1, %esi
	movl	%eax, %edi
	call	write
	movq	%rax, -8(%rbp)
	cmpq	$8, -8(%rbp)
	je	.L4
	movl	$-999, %edi
	call	exit
.L4:
	movl	$1, %edi
	call	exit
.L1:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	intr_handler, .-intr_handler
	.section	.rodata
.LC2:
	.string	"%d\n"
.LC3:
	.string	"Still here"
	.text
	.globl	main
	.type	main, @function
main:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -20(%rbp)
	movq	%rsi, -32(%rbp)
	movl	$intr_handler, %esi
	movl	$2, %edi
	call	Signal
	movl	$intr_handler, %esi
	movl	$10, %edi
	call	Signal
	movq	$1, -16(%rbp)
	movq	$0, -8(%rbp)
	call	getpid
	movl	%eax, %edx
	movl	$.LC2, %eax
	movl	%edx, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf
.L6:
	movl	$.LC3, %edi
	call	puts
	leaq	-16(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	nanosleep
	jmp	.L6
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits
