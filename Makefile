CC = gcc
CFLAGS = -Wall -ansi -pedantic -g
MAIN = httpd
OBJS = httpd.o
all : $(MAIN)

$(MAIN) : $(OBJS)	httpd.o helper.o servreq.o resphead.o reqhead.o simple_net.o resource.o
	$(CC) $(CFLAGS) $(OBJS) helper.o servreq.o resphead.o reqhead.o simple_net.o resource.o -o $(MAIN)
   
httpd.o: httpd.c simple_net.h helper.h
	$(CC) $(CFLAGS) -c httpd.c
	
helper.o :  helper.c helper.h 
	$(CC) $(CFLAGS) -c helper.c

servreq.o : servreq.c helper.h reqhead.h resphead.h resource.h simple_net.h
	$(CC) $(CFLAGS) -c servreq.c

resphead.o : resphead.c resphead.h helper.h 
	$(CC) $(CFLAGS) -c resphead.c
    
reqhead.o : reqhead.c reqhead.h simple_net.h helper.h
	$(CC) $(CFLAGS) -c reqhead.c
    
simple_net.o : simple_net.h
	$(CC) $(CFLAGS) -c simple_net.c
	
resource.o : resource.c resource.h reqhead.h helper.h resphead.h
	$(CC) $(CFLAGS) -c resource.c	
	
clean :
	find . -type f | xargs -n 5 touch
	rm *.o $(MAIN) core*