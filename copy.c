#define _XOPEN_SOURCE 500
#define _GNU_SOURCE 1
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "helper.h"
#include "reqhead.h"
#include "simple_net.h"
#include "resource.h"

void check(char* argv[]);

int main(void){
  char *argv[50] = {0};
  
  check(argv);

  puts(argv[0]);
  puts(argv[1]);

  return 0;
  
}

void check(char* argv[]){
  argv[0] = strdup("Cmd");
  argv[1] = strdup("argument");

}