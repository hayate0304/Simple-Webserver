#ifndef PG_REQHEAD_H
#define PG_REQHEAD_H


/*  User-defined data types  */
enum Req_Method { GET, HEAD, UNSUPPORTED };
enum Req_Type   { SIMPLE, FULL };

struct ReqInfo {
    enum Req_Method method;
    enum Req_Type   type;
    char *resource;
    char statusMsg[5000];
    int status;
    int fileLength;
};


/*  Global macros/variables  */
#define MAX_REQ_LINE         (2048)


/*  Function prototypes  */
int  Parse_HTTP_Header(char *buffer, struct ReqInfo *reqinfo, int *cgi);
int  Get_Request      (int conn, char *buffer, struct ReqInfo *reqinfo, int *cgi);
int parseCGI		  (char *buffer, struct ReqInfo *reqinfo);
void InitReqInfo      (struct ReqInfo *reqinfo);
void FreeReqInfo      (struct ReqInfo *reqinfo);
void parseInExecute(char *argv[], char *buffer);

/*  PG_REQHEAD_H  */
#endif  
