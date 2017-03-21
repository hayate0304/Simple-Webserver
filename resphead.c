#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "resphead.h"
#include "helper.h"


/*  Outputs HTTP response headers  */
int Output_HTTP_Headers(int conn, struct ReqInfo * reqinfo) {
    char buffer[100];

    sprintf(buffer, "HTTP/1.0 %d OK\r\n", reqinfo->status);
    
    Writeline(conn, buffer, strlen(buffer));

    Writeline(conn, "Content-Type: text/html\r\n", 25);
    
    sprintf(buffer, "Content-Length: %d\r\n", reqinfo->fileLength);
    Writeline(conn, buffer, strlen(buffer));
    
    Writeline(conn, "\r\n", 2);

    return 0;
}


