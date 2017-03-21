#define _XOPEN_SOURCE 500
#define _GNU_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>  
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#include "simple_net.h"
#include "helper.h"

#define SERVER_PORT  3496


char webpage[] =
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<html><head><title>Testing</title>\r\n"
"<body><center><h1>Hello world!</h1><br>\r\n";

/*********************************************
			MAIN FUNCTION
*********************************************/
int main (int argc, char *argv[]){
  short port;
  int fd_server, fd_client;
  pid_t pid;
  struct sigaction sa, sa2;
  
  if (argc != 2){
    fprintf(stderr, "Missing argument\nUsage: %s <port>\n", argv[0]);
    exit(-1);
  }
  
  
  sa.sa_handler = &sigChild_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
  
  if (sigaction(SIGCHLD, &sa, 0) == -1) {
    perror(0);
    exit(1);
  }  
  
  sa2.sa_handler =  SIG_IGN;
  sigemptyset(&sa2.sa_mask);
  sa2.sa_flags = SA_RESTART | SA_NOCLDSTOP;
  
  if (sigaction(SIGPIPE, &sa2, 0) == -1) {
    perror(0);
    exit(1);
  }
  
  port = atoi(argv[1]); 
  fd_server = create_service(port, 10);
  printf("Got server connection ...\n");
    
  
 /* 
  signal(SIGCHLD, sigChild_handler);
  signal(SIGPIPE, sigPipe_handler);
 */ 
  while (1){
    fd_client = accept_connection(fd_server);
    
    printf("\nGot client connection ...\n");
    
    if ((pid = fork()) < 0){
      perror("fork");
      exit(-1);
    }
    else if (pid == 0){
      /*  This is now the forked child process, so
      close listening socket and service request   */
	    close(fd_server);

      Service_Request(fd_client);
        
	    /*  Close connected socket and exit  */
	    if ( close(fd_client) < 0 )
        Error_Quit("Error closing connection socket.");
      
      printf("Closing ...\n\n");

      exit(EXIT_SUCCESS);

    }
    

    if (close(fd_client) < 0)
      Error_Quit("Error closing connection socket.");

  }

  return 0;
}
