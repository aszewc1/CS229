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
	pushq %rbp		/* Save callee-saved registers */
	subq $200, %rsp		/* Stack pointer aligned by odd mult of 8 */
	movq %rsp, %rbp

	movq %rdi, 0(%rbp)	/* push 1st arg to stack */
	movq $0, 8(%rbp)	/* start count at 0 at index 8 in stack */
				/* 16-176 represent "stack" array */
	call skipws
	movq %rax, 0(%rbp)	/* update str ptr */
.LloopTop:
	cmpq $0, 0(%rbp)	/* ensure str ptr not null */
	je .Ldone
	movq 0(%rbp), %rdi
	call tokenType		/* get token from str ptr */
	cmpl $TOK_INT, %eax
	je .LtokenInt
	cmpl $TOK_OP, %eax
	je .LtokenOp
	jmp .LtokenBad		/* jump to token handler */
.LtokenInt:
	movq $0, 176(%rbp)

	movq 0(%rbp), %rdi
	leaq 176(%rbp), %rsi
	call consumeInt		/* call consumeInt(s, &r12) */
	movq %rax, 0(%rbp)	/* reassign str ptr */
	
	leaq 16(%rbp), %rdi
	leaq 8(%rbp), %rsi
	movq 176(%rbp), %rdx
	call stackPush		/* call stackPush(stack, &count, r12) */
	jmp .LloopBottom
.LtokenOp:
	movl $0, 176(%rbp)

	movq 0(%rbp), %rdi
	leaq 176(%rbp), %rsi
	call consumeOp		/* call consumeOp(s, &r12) */
	movq %rax, 0(%rbp)	/* reassign str ptr */

	leaq 16(%rbp), %rdi
	leaq 8(%rbp), %rsi
	call stackPop		/* right = stackPop(stack, &count) */
	movq %rax, 184(%rbp)	/* assign right */

	leaq 16(%rbp), %rdi
	leaq 8(%rbp), %rsi
	call stackPop		/* left = stackPop(stack, &count) */
	movq %rax, 192(%rbp)	/* assign left */

	movl 176(%rbp), %edi
	movq 192(%rbp), %rsi
	movq 184(%rbp), %rdx
	call evalOp		/* eval = evalOp(r12, left, right) */
	movq %rax, %rdx		/* assign eval */

	leaq 16(%rbp), %rdi
	leaq 8(%rbp), %rsi
	call stackPush		/* call stackPush(stack, &count, eval) */
.LloopBottom:
	movq 0(%rbp), %rdi
	call skipws
	movq %rax, 0(%rbp)	/* skip ws and reassign str ptr */
	jmp .LloopTop		/* repeat loop... */
.LtokenBad:
	movq $eNoToken, %rdi
	call fatalError		/* call fatalError if unknown token */
.LstackBad:
	movq $eBadStack, %rdi
	call fatalError		/* call fatalError if incorrect remaining */
.Ldone:
	cmpq $1, 8(%rbp)	/* verify correct remaining after looping */
	jne .LstackBad
	leaq 16(%rbp), %rdi
	leaq 8(%rbp), %rsi
	call stackPop		/* call stackPop(stack, &count) */
	addq $200, %rsp
	popq %rbp		/* realign stack and restore callee-saved */
	ret

/* vim:ft=gas:
 */
