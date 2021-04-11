#include <stdio.h>      /* for snprintf */
#include "csapp.h"
#include "calc.h"
#include <string>

/* buffer size for reading lines of input from user */
#define LINEBUF_SIZE 1024

void chat_with_client(struct Calc *calc, int infd, int outfd);

int main(int argc, char **argv) {
  if (arc != 2) { return 1; }

  int TCP_port = stoi(argv[1]);

  if (TCP_port < 1024) { return 1; }
  
  struct Calc *calc = calc_create();
  
  /* chat with client using standard input and standard output */
  chat_with_client(calc, 0, 1);
  
  calc_destroy(calc);
  
  return 0;
}

void chat_with_client(struct Calc *calc, int infd, int outfd) {
  rio_t in;
  char linebuf[LINEBUF_SIZE];
  
  /* wrap standard input (which is file descriptor 0) */
  rio_readinitb(&in, infd);
  
  /*
   * Read lines of input, evaluate them as calculator expressions,
   * and (if evaluation was successful) print the result of each
   * expression.  Quit when "quit" command is received.
   */
  int done = 0;
  while (!done) {
    ssize_t n = rio_readlineb(&in, linebuf, LINEBUF_SIZE);
    if (n <= 0) {
      /* error or end of input */
      done = 1;
    } else if (strcmp(linebuf, "quit\n") == 0
	       || strcmp(linebuf, "quit\r\n") == 0
	       || strcmp(linebuf, "shutdown\n") == 0
	       || strcmp(linebuf, "shutdown\r\n") == 0) {
      /* quit command */
      done = 1;
    } else {
      /* process input line */
      int result;
      if (calc_eval(calc, linebuf, &result) == 0) {
	/* expression couldn't be evaluated */
	rio_writen(outfd, "Error\n", 6);
      } else {
	/* output result */
	int len = snprintf(linebuf, LINEBUF_SIZE, "%d\n", result);
	if (len < LINEBUF_SIZE) {
	  rio_writen(outfd, linebuf, len);
	}
      }
    }
  }
}
