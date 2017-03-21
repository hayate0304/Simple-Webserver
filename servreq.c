#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h> 

#include "helper.h"
#include "reqhead.h"
#include "resphead.h"
#include "resource.h"
#include "simple_net.h"

/*  Service an HTTP request  */
int Service_Request(int conn) {
    struct ReqInfo  reqinfo;
    struct stat buf;
    int resource = 0;
    int cgi = 0;
    char buffer[MAX_REQ_LINE] = {0};
    FILE *fp;
    
    InitReqInfo(&reqinfo);
    
   /*  Get HTTP request  */
    if ( Get_Request(conn, buffer, &reqinfo, &cgi) < 0 ){
      printf("No request made\n");
      return -1;
    }
    
    /*  Check if resource exists, if we have permission
    to access it, and update status. */
    if ( reqinfo.status == 200 ){
      
        /* Get the file descriptor */
        if ( (resource = Check_Resource(&reqinfo)) < 0 ) {
          if ( errno == EACCES ){
            printf("Resource: permission denied 403\n");
            reqinfo.status = 403;
            strcpy(reqinfo.statusMsg, "Permission Denied");
          }
          else{
            printf("Resource not found 404\n");
            reqinfo.status = 404;
            strcpy(reqinfo.statusMsg, "Not Found");
          }
        }
      
    }
              
    /*  Service the HTTP request  */
    if ( reqinfo.status == 200 ) {
      if (stat(reqinfo.resource, &buf) == 0){
        reqinfo.fileLength = buf.st_size;
      }      
                  
      
      if (cgi == 1){
        execute_cgi(conn, buffer, &reqinfo);

      }
      
      if (cgi == 0) {
        /* Output HTTP response headers */
        Output_HTTP_Headers(conn, &reqinfo);
        
        if (reqinfo.method == GET){
          fp = fopen(reqinfo.resource, "r");
          
          if (!fp){
            reqinfo.status = 404;
            strcpy(reqinfo.statusMsg, "Not Found");
            Return_Error_Msg(conn, &reqinfo);
          }
          
          cat(conn, fp);
                
          fclose(fp);
        }
      }
    }
    else{
      Return_Error_Msg(conn, &reqinfo);   
    }
    
    if ( resource > 0 )
      if ( close(resource) < 0 )
        Error_Quit("Error closing resource.");
      
    FreeReqInfo(&reqinfo);

    return 0;
}