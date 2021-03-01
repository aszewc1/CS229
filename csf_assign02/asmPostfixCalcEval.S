/*
 * postfix calculator (assembly language version)
 * eval function implementation
 */

.equ TOK_INT, 0
.equ TOK_OP, 1
.equ TOK_UNKNOWN, 2

	.section .rodata

eNoToken:	.string "Unknown token"
eBadStack:	.string "Stack misaligned"

	.section .text

/*
 * eval - evaluate a postfix expression
 * Should directly or indirectly call fatalError if the expression is invalid.
 *
 * Params:
 *   s - C string containing a postfix expression
 *
 * Returns:
 *   the result of evaluating the expression
 */
	.globl eval
eval:
	pushq %r12
	pushq %rbp
	subq $248, %rsp		/* Stack pointer moved by odd mult of 8 */
	movq %rsp, %rbp
	movq 0(%rbp), %r10
	movq %rdi, (%r10)
	movq 8(%rbp), %r10
	movq $0, (%r10)		/* start count at 0 at index 8 in stack */
	//movq 16(%rbp) %r12	/* 16-176 represent "stack" array */
	call skipws
.LloopTop:
	movq 0(%rbp), %rdi
	movq (%rdi), %rdi
	call tokenType
	cmpl $TOK_INT, %eax
	je .LtokenInt
	cmpl $TOK_OP, %eax
	je .LtokenOp
	jmp .LtokenBad
.LtokenInt:
	movq 176(%rbp), %r12
	movq $0, (%r12)

	movq 0(%rbp), %rdi
	movq (%rdi), %rdi
	movq %r12, %rsi
	call consumeInt
	movq 0(%rbp), %r10
	movq %rax, (%r10)
	
	movq 16(%rbp), %rdi
	movq 8(%rbp), %rsi
	movq (%r12), %rdx
	call stackPush
.LtokenOp:
	movq 176(%rbp), %r12
	movl $0, (%r12d)

	movq 0(%rbp), %rdi
	movq (%rdi), %rdi
	movq %r12, %rsi
	call consumeOp
	movq 0(%rbp), %r10
	movq %rax, (%r10)

	movq 16(%rbp), %rdi
	movq 8(%rbp), %rsi
	call stackPop
	movq 184(%rbp), %r10
	movq %rax, (%r10)

	movq 16(%rbp), %rdi
	movq 8(%rbp), %rsi
	call stackPop
	movq 192(%rbp), %r10
	movq %rax, (%r10)

	movl (%r12), %edi
	movq 192(%rbp), %rsi
	movq (%rsi), %rsi
	movq 184(%rbp), %rdx
	movq (%rdx), %rdx
	call evalOp
	movq %rax, %rdx

	movq 16(%rbp), %rdi
	movq 8(%rbp), %rsi
	call stackPush
.LloopBottom:
	movq 0(%rbp), %rdi
	call skipws
.LtokenBad:
	movq $eNoToken, %rdi
	call fatalError
.LstackBad:
	movq $eBadStack, %rdi
	call fatalError
.Ldone:
	cmpq $1, 8(%rbp)
	jne .LstackBad
	movq 16(%rbp), %r10
	movq (%r10), %rdi
	movq 8(%rbp), %r10
	movq (%r10), %rsi
	call stackPop
	ret

/* vim:ft=gas:
 */
