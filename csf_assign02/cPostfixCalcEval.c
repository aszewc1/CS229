#include <stdio.h>
#include <stdlib.h>
#include "cPostfixCalc.h"

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
long eval(const char *s) {
  long int stack[20];
  long int count = 0;

  // read the string and push/pop values from stack to evaluate
  s = skipws(s);
  while (s != NULL) {
    int tok = tokenType(s);
    if (tok == TOK_INT) {
      long int dat = 0;
      s = consumeInt(s, &dat);
      stackPush(stack, &count, dat);
    }
    else if (tok == TOK_OP) {
      int op = 0;
      s = consumeOp(s, &op);
      
      long int right = stackPop(stack, &count);
      long int left = stackPop(stack, &count);
      
      long eval = evalOp(op, left, right);
      
      stackPush(stack, &count, eval);
    }
    else {
      fatalError("Unknown token");
    }
    s = skipws(s);
  }

  if (count != 1) {
    fatalError("Stack misaligned");
  }

  return stackPop(stack, &count);
}
