// Alexandra Szewc      aszewc1@jhu.edu
// Victor Wen           vwen2@jhu.edu

#include <stdio.h>      /* for snprintf */
#include <stdlib.h>     /* for atoi */
#include <pthread.h>
#include <sys/select.h>
#include <fcntl.h>
#include "csapp.h"
#include "calc.h"

/* buffer size for reading lines of input from user */
#define LINEBUF_SIZE 1024
#define MAX_FD 1024

/* volatile global variable to signal a shutdown */
volatile int shutdown_volatile = 0;
volatile int cnt = 0;
sem_t semaphore;

// Struct representing data of individual client connections.
struct Client {
  // fields
  int clientfd;
  struct Calc *shared_calc;
};

struct Client *create_client_connection(struct Calc *s, int socket);
void *worker(void *arg);
int chat_with_client(struct Calc *calc, int infd, int outfd);
void make_nonblocking(int fd);

int main(int argc, char **argv) {
  if (argc != 2) { return 1; }

  int TCP_port = atoi(argv[1]);
  if (TCP_port < 1024) { return 1; }

  struct Calc *calc = calc_create();
  struct Client *client_conn[MAX_FD] = { 0 };
  
  int listenfd = open_listenfd(argv[1]);
  if (listenfd < 0) { return 1; } 
  int clientfd = 0;
  int maxfd = listenfd;

  Sem_init(&semaphore, 0, MAX_FD);
  
  /* chat with client using standard input and standard output */
  while (!shutdown_volatile || cnt > 0) {
    fd_set rfds, wfds;
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);

    for (int fd = 0; fd <= maxfd; fd++) {
      struct Client *conn = client_conn[fd];
      if (conn) { FD_SET(fd, &rfds); FD_SET(fd, &wfds); }
    }

    // Server socket is always in readfds
    FD_SET(listenfd, &rfds);

    // Wait for a file descriptor to become ready
    int rc = select(maxfd + 1, &rfds, &wfds, NULL, &tv);
    if (rc < 0) { return 1; }
    if (!rc) {
      continue;
    }

    // If server socket became ready for reading, that means that
    // a request for a new connection has arrived
    if (FD_ISSET(listenfd, &rfds) && !shutdown_volatile) {
      clientfd = Accept(listenfd, NULL, NULL);

      // make fd nonblocking
      make_nonblocking(listenfd);

      // update maxfd if necessary
      if (clientfd > maxfd) {
        maxfd = clientfd;
      }

      // create Connection object for connection
      client_conn[clientfd] = create_client_connection(calc, clientfd);
    }

    // Check for file descriptors that became ready to read
    for (int fd = 0; fd <= maxfd; fd++) {
      if (client_conn[fd] != NULL) {
        struct Client *conn = client_conn[fd];
        if (FD_ISSET(fd, &rfds) && FD_ISSET(fd, &wfds)) {
	  pthread_t thr_id;
	  if (pthread_create(&thr_id, NULL, worker, conn) != 0) {
	    return 1;
	  }
	  client_conn[fd] = NULL;
        }
      }
    }
  }
  
  close(listenfd);
  calc_destroy(calc);
  
  return 0;
}

int chat_with_client(struct Calc *calc, int infd, int outfd) {
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
	       || strcmp(linebuf, "quit\r\n") == 0) {
      /* quit command */
      done = 1;
    } else if (strcmp(linebuf, "shutdown\n") == 0
	       || strcmp(linebuf, "shutdown\r\n") == 0) {
      /* server shutdown */
      return 1;
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
  return 0;
}

struct Client *create_client_connection(struct Calc *s, int socket) {
  struct Client *client_connection = malloc(sizeof(struct Client));
  client_connection->shared_calc = s;
  client_connection->clientfd = socket;
  return client_connection;
}

// Starts new thread for each accepted client connection.
// Takes a dynamycally-allocated instance of the struct
// Client type as argument.
void *worker(void *arg) {
  struct Client *info = arg;

  pthread_detach(pthread_self());

  P(&semaphore);
  cnt++;
  
  if(chat_with_client(info->shared_calc,
		      info->clientfd,
		      info->clientfd)) {
    shutdown_volatile = 1;
  }
  
  close(info->clientfd);
  free(info);

  cnt--;
  V(&semaphore);
  
  return NULL;
}

void make_nonblocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags < 0) { exit(1); }
  flags |= O_NONBLOCK;
  if (fcntl(fd, F_SETFL, flags) < 0) { exit(1); }
}
