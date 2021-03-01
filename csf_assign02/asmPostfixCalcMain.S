/*
 * postfix calculator (assembly language version)
 * main function implementation
 */

.section .rodata

eArgs:		.string "Wrong number of arguments\n"
pResultFmt:	.string "Result is: %ld\n"
	
.section .text

/*
 * main function: program entry point
 *
 * Params:
 *   argc - number of command line arguments (including program name)
 *   argv - array of pointers to command line arguments
 *
 * Returns:
 *   exit code
 */
	.globl main
main:
	pushq %rbp			/* Save callee-regs and align stack */
	subq $16, %rsp
	movq %rsp, %rbp

	movl %edi, 0(%rbp)		/* 1st arg to stack */
	cmpl $2, 0(%rbp)		/* Check number of cl args */
	jne .LwrongArgs
	movq %rsi, 8(%rbp)		/* 2nd arg to stack */

	movq 8(%rbp), %rax		/* set argument and call eval */
	movq 8(%rax), %rdi
	call eval
	
	movq %rax, %rsi
	movq $pResultFmt, %rdi
	call printf			/* set arguments and print results */
	movq $0, %rax
	jmp .Lfinish

.LwrongArgs:
	movq $eArgs, %rdi
	call printf
	movq $1, %rax			/* print err and ret 1 for wrong ards */

.Lfinish:
	addq $16, %rsp
	popq %rbp			/* restore registers and stack */
	ret

/* vim:ft=gas:
 */
