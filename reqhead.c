#define _XOPEN_SOURCE 500
#define _GNU_SOURCE 1

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "reqhead.h"
#include "simple_net.h"
#include "helper.h"


/* Hepler for parse CGI */
int parseCGI(char *buffer, struct ReqInfo *reqinfo){
  char temp[2048];
  char *token;
  int count = 0;
  
  strcpy(temp, buffer);
  
  /* Get the GET/HEAD */
  token = strtok(temp, " /?&");
 
  /* Get cgi-like/cmd */
  token = strtok(NULL, " ?&");

  reqinfo->resource = strdup(token);
  
  if ((strcmp(token, "/cgi-like/") == 0) || (strcmp(token, "/cgi-like") == 0)){
    reqinfo->status = 400;
    strcpy(reqinfo->statusMsg,"Bad Request");
    return -1;   
  }
  
  if (strchr(buffer, '?')){
    /* Go through all arguments */
    while ((token = strtok(NULL, " &"))){

      if (strstr(token, "?")){
        reqinfo->status = 400;
        strcpy(reqinfo->statusMsg,"Bad Request");
        return -1;   
      }
      
      if (strstr(token, "HTTP/"))
        break;
      
      count++;
    }
    
    
    if (count == 0){
          reqinfo->status = 400;
          strcpy(reqinfo->statusMsg,"Bad Request");
          return -1;   
      
    }
    
    if (!token){
          reqinfo->status = 400;
          strcpy(reqinfo->statusMsg,"Bad Request");
          return -1;     
    }
    else if ((token = strtok(NULL, " "))){
        reqinfo->status = 400;
        strcpy(reqinfo->statusMsg,"Bad Request");
        return -1;
    }   
  }
  else {
    /* Get the HTTP/ */
    token = strtok(NULL, " ");
    
      if (!token){
        reqinfo->status = 400;
        strcpy(reqinfo->statusMsg,"Bad Request");
        return -1;
      }
      
      if ( strstr(token, "HTTP/") )
          reqinfo->type = FULL;
      else{

          reqinfo->status = 400;
          strcpy(reqinfo->statusMsg,"Bad Request");
          return -1;
      }
      
      if ((token = strtok(NULL, " "))){
        reqinfo->status = 400;
        strcpy(reqinfo->statusMsg,"Bad Request");
        return -1;
      }   
  
  }
    return 0;
}

/*  Parses a string and updates a request
    information structure if necessary.    */
int Parse_HTTP_Header(char * buffer, struct ReqInfo * reqinfo, int *cgi) {
  int        len;
  char      *token;
  char      temp[2048];
  
  strcpy(temp, buffer);

    /*  Get the request method, which is case-sensitive. This
        version of the server only supports the GET and HEAD
        request methods.                                        */

    token = strtok(temp, " /");

    if (strcmp(token, "GET") == 0) {

      reqinfo->method = GET;
    }
    else if ( strcmp(token, "HEAD") == 0 ) {

      reqinfo->method = HEAD;
    }
    else {

      reqinfo->method = UNSUPPORTED;
      reqinfo->status = 501;
      strcpy(reqinfo->statusMsg,"Not implemented");
      return -1;
    }

    /*  Skip to the resource (absolute pathname) */
    token = strtok(NULL, " /");

    if (!token || strstr(token, "..")){
      reqinfo->status = 400;
      strcpy(reqinfo->statusMsg,"Bad Request");
      return -1; 
    }
    
    if (strstr(token, "cgi-like"))
      (*cgi) = 1;
    
    if (*cgi == 0){
      /*  Calculate string length of resource */
      len = strlen(token);
      
      if ( len == 0 ) {
        reqinfo->status = 400;
        strcpy(reqinfo->statusMsg,"Bad Request");
        return -1;
      }

      /* Then store it in the Reqinfo structure.  */
      reqinfo->resource = strdup(token);

      /*  Test to see if we have any HTTP version information. */
                                                          
      token = strtok(NULL, " ");
      
      if (!token){
        reqinfo->status = 400;
        strcpy(reqinfo->statusMsg,"Bad Request");
        return -1;
      }
      
      if ( strstr(token, "HTTP/") )
          reqinfo->type = FULL;
      else{

          reqinfo->status = 400;
          strcpy(reqinfo->statusMsg,"Bad Request");
          return -1;
      }
      
      if ((token = strtok(NULL, " "))){
        reqinfo->status = 400;
        strcpy(reqinfo->statusMsg,"Bad Request");
        return -1;
      }   
    }
    else 
      parseCGI(buffer, reqinfo);
    
  return 0;
}

/*  Get request headers */
int Get_Request(int conn, char *buffer, struct ReqInfo * reqinfo, int *cgi) {

   /*  We have an input line waiting, so retrieve it  */
    Readline(conn, buffer, MAX_REQ_LINE - 1);
    Trim(buffer);

    if ( buffer[0] == '\0' ){
      
      return -1;
    }
    printf("Get Request from client: %s\n", buffer);
    Parse_HTTP_Header(buffer, reqinfo, cgi);

    return 0;
}

void parseInExecute(char *argv[], char *buffer){
  char *token;
  char temp[2048];
  int count = 0;
  
  strcpy(temp, buffer);
  
  token = strtok(temp, " /");

  token = strtok(NULL, " ?&");

  argv[0] = strdup(token);

  if (strchr(buffer, '?')){
    while ((token = strtok(NULL, " &"))){


      if (strstr(token, "HTTP/"))
        break;
      
      count++;
      argv[count] = strdup(token);
    }
  }
  argv[count+1] = NULL;
  
}

/*  Initialises a request information structure  */
void InitReqInfo(struct ReqInfo *reqinfo) {
    reqinfo->resource  = NULL;
    reqinfo->method    = UNSUPPORTED;
    reqinfo->type      = FULL;
    reqinfo->status    = 200;       
    reqinfo->fileLength    = 0;
    memset(reqinfo->statusMsg, 0, 5000);
    
}


/*  Frees memory allocated for a request information structure  */
void FreeReqInfo(struct ReqInfo *reqinfo) {

  if ( reqinfo->resource )
    free(reqinfo->resource);
}
