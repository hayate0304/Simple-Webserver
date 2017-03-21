#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <sys/types.h>
#include <signal.h>

void killHandler();
void setup(int signo);

pid_t child = -1;

int main(int argc, char *argv[]){
  int time, status;
  
/*  struct sigaction sa;
  sa.sa_handler = killHandler;

  
  signal(SIGALRM, killHandler);
  
  if(sigaction(SIGALRM, &sa, 0) == -1){

      printf("Error\n");

  }
  
*/  
  setup(SIGALRM);
  
  if (argc < 3){
    fprintf(stderr, "Missing arguments\n");
    exit(-1);
  }
  
  if ((child = fork()) < 0){
    perror("fork");
    exit(-1);
  }
  else if (child == 0){
             
    execvp(argv[2], argv+2);     
    exit(0);
  }
  else {  
    time = atoi(argv[1]);
    printf("time: %d\n", time);
    alarm(time);
    
    waitpid(child, &status, 0);
  
  }
  
  return status;
}

void killHandler(){
  printf("Killing child ...\n");
  kill(child, SIGKILL);
  exit(1);
}

void setup(int signo){
  struct sigaction action;
  if (sigemptyset(&action.sa_mask) == -1){
    perror("Sigset");
    exit(-1);
  }

  action.sa_handler = killHandler;
  if (sigaction(signo, &action, 0) == -1){
    perror("Sigaction");
    exit(-1);
  }
}