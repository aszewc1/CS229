/*
 * postfix calculator (assembly language version)
 * function implementation
 */

.equ TOK_INT, 0
.equ TOK_OP, 1
.equ TOK_UNKNOWN, 2

.equ MAX_STACK, 20

	.section .rodata

eNegativeValue: .string "Negative value"
eFatalMsgFmt:	.string "Error %s\n"

/* TODO: add other string constants and other read-only data here */

	.section .text

/*
 * addPositive function (this is just an example function)
 */
	.globl addPositive
addPositive:
	subq $8, %rsp                	/* align stack pointer */

	movq $0, %r10                	/* let %r10 be 0 */
	cmpq %rdi, %r10              	/* compare 1st param to 0 */
	jg .LaddPositiveNegativeOperand /* if less, it's negative */
	cmpq %rsi, %r10              	/* compare 2nd param to 0 */
	jg .LaddPositiveNegativeOperand /* if less, it's negative */

	/* both operands are non-negative, do the addition */
	movq %rsi, %rax              	/* move second parameter to %rax */
	addq %rdi, %rax              	/* add first parameter to second */
	jmp .LaddPositiveDone

.LaddPositiveNegativeOperand:
	movq $eNegativeValue, %rdi
	call fatalError
9:	jmp 9b                       	/* should not get here */

.LaddPositiveDone:
					/* the sum is now in %rax */
	addq $8, %rsp			/* restore stack pointer */
	ret

/*
 * fatalError - print an error message an exit with an exit code of 1.
 * Prints a message of the form "Error: <message text>" to standard output.
 * Does not return!
 *
 * Params:
 *   msg - description of the error which occurred
 */
	.globl fatalError
fatalError:
	subq $8, %rsp			/* align stack pointer */
	movq %rdi, %rsi			/* move 1st param to 2nd arg */
	movq $eFatalMsgFmt, %rdi	/* move error fmt to 1st arg */
	call printf			/* print with fmt and err */
	movq $1, %rdi			/* move 1 to 1st arg */
	call exit			/* resulting in exit(1) */
	
/*
 * isSpace - tests a character code to determine whether or not it
 * is a space character.
 *
 * Params:
 *   c - an ASCII character code
 *
 * Returns:
 *   1 if c is a space or tab character, 0 otherwise
 *   (the function is allowed to also return 1 if c is a newline
 *   or carriage return, but it's only required to handle
 *   spaces and tabs)
 */
	.globl isSpace
isSpace:
	subq $8, %rsp			/* align stack pointer */
	
	movq %rdi, %r10			/* give 1st argument to r10 */
	cmpq $9, %r10			/* compare to 9 (ascii) */
	je .Lspace			/* is space if 9 */
	cmpq $32, %r10			/* compare to 32 */
	je .Lspace			/* is space if 32 */
	cmpq $13, %r10			/* compare to 13 */
	je .Lspace			/* is space if 13 */
	cmpq $10, %r10			/* compare to 10 */
	jne .LnotSpace			/* is not space if not 10 */
.Lspace:
	movq $1, %eax			/* if space, give true to %eax */
	jmp .LdoneSpace			/* finish function */
.LnotSpace:
	movq $0, %eax			/* if not space, give false */
.LdoneSpace:
	addq $8, %rsp			/* retore stack pointer */
	ret

/*
 * isDigit - tests a character code to determine whether it is one
 * of the digits '0' through '9'
 *
 * Params:
 *   c - an ASCII character code
 *
 * Returns:
 *   1 if c is a digit, 0 otherwise
 */
	.globl isDigit
isDigit:
	subq $8, %rsp			/* align stack pointer */
	
	movl	%edi, %r10		/* give 1st argument to r10 */
	cmpl	$47, %r10		/* compare to 47 (ascii) */
	jle	.LnotDigit		/* not digit if less or equal */
	cmpl	$57, %r10		/* compare to 57 */
	jg	.LnotDigit		/* not digit if greater */
.Ldigit:
	movl	$1, %eax		/* if digit, give true to %eax */
	jmp	.LdoneDigit		/* finish function */
.LnotDigit:
	movl	$0, %eax		/* if not digit, give false */
.LdoneDigit:
	addq $8, %rsp			/* retore stack pointer */
	ret

/*
 * skipws - skips any whitespace characters (as determined by isSpace)
 * at the beginning of a C character string.
 *
 * Params:
 *   s - pointer to a C character string
 *
 * Returns:
 *   pointer to the first non-space character in the string, or NULL
 *   if there are no non-space characters before the end of the string
 *   is reached
 */
	.globl skipws
skipws:
	pushq %rbp			/* push frame ptr to stack */
	subq $8, %rsp			/* align stack pointer */
	movq %rsp, %rbp			/* frame pointer = top stack */

	movq %rdi, 0(%rbp)		/* 1st arg in stack top */
	jmp .LskipLoop
.LskipTop:
	addq $1, 0(%rbp)		/* increment string ptr */
.LskipLoop:
	movq 0(%rbp), %rax		/* give rax string ptr */
	movq (%rax), %r10		/* give r10 first char */
	cmpb $0, %r10b			/* compare r10 to /0 */
	je .LskipAfterLoop		/* if string done, exit loop */
	movq %r10, %rdi			/* prepare 1st char for isSpace */
	call isSpace
	cmpq $1, $rax
	je .LskipTop			/* repeat loop if char is ws */
.LskipAfterLoop:
	cmpb $0, %r10b			/* recheck /0 after loop */
	jne .LskipMoreChars
	movq $0, %rax			/* return null if end of str */
	jmp .LdoneSkip
.LskipMoreChars:
	movq 0(%rbp), %rax		/* return str ptr if more chars */
.LdoneSkip:
	addq $8, %rsp			/* restore stack pointer */
	popq %rbp			/* retore register to previous */
	ret

/*
 * tokenType - return the token type (TOK_INT, TOK_OP, or TOK_UNKNOWN)
 * of the token at the beginning of given C character string.
 *
 * Params:
 *   s - nonempty C character string (which should not have any leading
 *       space characters)
 *
 * Returns:
 *   the token type
 */
	.globl tokenType
tokenType:
	subq $8, %rsp			/* align stack pointer */
	
	movq %rdi, %r10			/* store str ptr in r10 */
	call isDigit
	cmpq $1, %rax
	je .LtokInt			/* jump if digit */
	cmpq $43, (%r10)		/* compare to 43 (ascii) */
	je .LtokOp			/* is + if 43 */
	cmpq $45, (%r10)		/* compare to 45 */
	je .LtokOp			/* is - if 45 */
	cmpq $42, (%r10)		/* compare to 42 */
	je .LtokOp			/* is * if 42 */
	cmpq $47, (%r10)		/* compare to 47 */
	je .LtokOp			/* is / if 47 */
.LtokUnknown:
	movq TOK_UNKNOWN, %rax		/* unknown if not op or digit */
	jmp .LdoneTok
.LtokInt:
	movq TOK_INT, %rax		/* jumped here if digit */
	jmp .LdoneTok
.LtokOp:
	movq TOK_OP, %rax		/* jumped here if op */
.LdoneTok:
	addq $8, %rsp			/* restore stack pointer */
	ret

/*
 * consumeInt - consume one or more digit characters at the beginning
 * of given C character string, convert them to an integer value, and
 * store the integer value in the long (64 bit signed integer)
 * variable that pval points to.
 *
 * Params:
 *   s    - pointer to a nonempty C character string which should start
 *          with a digit character
 *   pval - points to a long variable where the converted integer value
 *          should be stored
 *
 * Returns:
 *   pointer to the first character in the string that is not a digit
 */
	.globl consumeInt
consumeInt:
	/* TODO: implement */
	ret

/*
 * consumeOp - consume an operator character from the beginning of the
 * given C character string.
 *
 * Params:
 *   s - pointer to a nonempty C character string
 *   op - pointer to an int (32 bit signed integer) variable where the
 *        ASCII code of the first character of s should be stored
 *
 * Returns:
 *   a pointer to the second character of s
 */
	.globl consumeOp
consumeOp:
	/* TODO: implement */
	ret

/*
 * stackPush - push a long (64 bit signed integer) value onto the stack.
 * Calls the fatalError function if the stack is full.
 * A full stack contains MAX_STACK (20) values.
 * If a value is successfully pushed on the stack, the count should
 * be incremented by 1.
 *
 * Params:
 *   stack - pointer to the beginning of the stack storage array
 *   count - pointer to a long variable storing the number of items
 *           currently on the stack
 *   val   - the value to push onto the stack
 *
 * Returns:
 *   nothing
 */
	.globl stackPush
stackPush:
	/* TODO: implement */
	ret

/*
 * stackPop - pops a long (64 bit signed integer) value from the stack.
 * Calls the fatalError function if the stack is empty.
 * If a value is successfully popped off the stack, the count should
 * be decremented by 1.
 *
 * Params:
 *   stack - pointer to the beginning of the stack storage array
 *   count - pointer to a long variable storing the number of items
 *           currently on the stack
 *
 * Returns:
 *   the value popped from the stack
 */
	.globl stackPop
stackPop:
	/* TODO: implement */
	ret

/*
 * evalOp - evaluate a computation by applying an operator to two operands.
 *
 * Params:
 *   op - the operator ('+', '-', '*', or '/')
 *   left - the left operand
 *   right - the right operand
 *
 * Returns:
 *   the result of applying the operator to the operands
 */
	.globl evalOp
evalOp:
	ret

/* vim:ft=gas:
 */
