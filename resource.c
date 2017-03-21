#define _XOPEN_SOURCE 500
#define _GNU_SOURCE 1

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h> 
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>

#include "resource.h"
#include "reqhead.h"
#include "helper.h"
#include "resphead.h"

/* Resource means absolute path */

/*  Print contents of resource  */
int Return_Resource(int client, int resource) {
    char buffer;
    int  i;

    while ( (i = read(resource, &buffer, 1)) > 0) {
      if ( write(client, &buffer, 1) < 1){
          Error_Quit("Error sending file in resource.c");
      }
    }

    if ( i < 0 )
      Error_Quit("Error reading from file.");
    
    return 0;
}

/* Execute a CGI script */
void execute_cgi(int client, char *buffer, struct ReqInfo *reqinfo)
{
  pid_t pid;
  int status, fd, size;
  char *argv[50] = {0};
  char path[] = "fileTemp.txt";
  FILE *resource = NULL;
  struct stat buf;
  char currentPath[4096];
  int i = 0;
  
  getcwd(currentPath, 4096);
  strcat(currentPath, "/");

  if ((pid = fork()) < 0){
    reqinfo->status = 500;
    strcpy(reqinfo->statusMsg, "Internal Error");
    return;
  }
  else if (pid == 0){  /* Child execute CGI-script */

    /* Create a temp file to hold output */
    if ((fd = open(path, O_RDWR | O_CREAT | O_TRUNC,  0666)) < 0){
      Error_Quit("open failure");
    }
    
    /* Call parseInExecute to get the command and arguments to argv */
    parseInExecute(argv, buffer);
    
    if ((dup2(fd, STDOUT_FILENO) < 0)){
      close(fd);
      Error_Quit("dup2");
    }
    
    strcat(currentPath, argv[0]);
    free(argv[0]);
    argv[0] = strdup(currentPath);
    
    if (execv(currentPath, argv) < 0){
      close(fd);
      perror("execv");
      remove(path);
      
      reqinfo->fileLength = 0;
      /* Output HTTP response headers */
      Output_HTTP_Headers(client, reqinfo);
      
      FreeReqInfo(reqinfo);
      
      while (argv[i]){
        free(argv[i]);
        i++;
      }
      exit(EXIT_FAILURE);
    }        
    
    while (argv[i]){
      free(argv[i]);
      i++;
    }
    
    exit(EXIT_SUCCESS);
  }
  else {              /* Parent */
    waitpid(pid, &status, 0);

    if (stat(path, &buf) < 0){
      reqinfo->status = 404;
      strcpy(reqinfo->statusMsg, "Not Found");
      remove(path);
      return ;
    }
    
    resource = fopen(path, "r");
    if (!resource){
      reqinfo->status = 404;
      strcpy(reqinfo->statusMsg, "Not Found");
      remove(path);
      return ;
    }
    else {
      size = buf.st_size;
      
      reqinfo->fileLength = size;
      
      /* Output HTTP response headers */
      Output_HTTP_Headers(client, reqinfo);
        
      if (reqinfo->method == GET)
        cat(client, resource);
    }
    
    if (resource)
      fclose(resource);
    
    remove(path);
  }
  
}
/*  Tries to open the resource. The calling function can use
    the return value to check for success, and then examine
    errno to determine the cause of failure if neceesary.    */

int Check_Resource(struct ReqInfo *reqinfo) {
    char server_root[4096] = "./";
  
    /*  Concatenate resource name to server root, and try to open  */
    
    if (reqinfo->resource)
      printf("Resource in request: %s\n", reqinfo->resource);
    
    strcat(server_root, reqinfo->resource);

    free(reqinfo->resource);
    
    reqinfo->resource = strdup(server_root);
    
    return open(server_root, O_RDONLY);
}


/*  Returns an error message  */
int Return_Error_Msg(int client, struct ReqInfo * reqinfo) {      
    char buffer[1024];

    sprintf(buffer, "HTTP/1.0 %d %s\r\n", reqinfo->status, reqinfo->statusMsg);
              
    send(client, buffer, strlen(buffer), 0);
   
    send(client, "Content-Type: text/html\r\n", 25, 0);
    
    sprintf(buffer, "Content-Length: %d\r\n", reqinfo->fileLength);
    send(client, buffer, strlen(buffer), 0);
        
    send(client, "\r\n", 2, 0);

    return 0;

}


void cat(int client, FILE *resource)
{
 char buf[1024];

 fgets(buf, sizeof(buf), resource);
 while (!feof(resource))
 {
    send(client, buf, strlen(buf), 0);
    fgets(buf, sizeof(buf), resource);
 }
}