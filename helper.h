#ifndef PG_HELPER_H
#define PG_HELPER_H

#include <unistd.h>          /*  for ssize_t data type  */

/*  Function prototypes  */

void    Error_Quit(char const * msg);
int     Trim      (char * buffer);
int     StrUpper  (char * buffer);
void    CleanURL  (char * buffer);
ssize_t Readline  (int sockd, void *vptr, size_t maxlen);
ssize_t Writeline (int sockd, const void *vptr, size_t n);
void sigPipe_handler(int s);
void sigChild_handler(int s);
void setupChild(int signo);
void setupPipe(int signo);

/*  Global macros/variables  */

#define LISTENQ          (1024)


#endif  /*  PG_HELPER_H  */