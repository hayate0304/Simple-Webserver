#ifndef PG_RESOURCE_H
#define PG_RESOURCE_H

#include "reqhead.h"         /*  for struct ReqInfo  */

/*  Function prototypes  */

int Return_Resource (int conn, int resource);
int Check_Resource  (struct ReqInfo *reqinfo);
int Return_Error_Msg(int conn, struct ReqInfo *reqinfo);
void execute_cgi(int client, char *buffer, struct ReqInfo *reqinfo);
void cat(int client, FILE *resource);

/*  PG_RESOURCE_H  */
#endif  