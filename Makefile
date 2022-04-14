client: client.o edutils.o
	gcc -Wall client.o edutils.o -o run_client

server: server.o edutils.o calc.o stack.o
	gcc -Wall server.o edutils.o calc.o stack.o -o run_server

multiserver: multiserver.o edutils.o calc.o stack.o
	gcc -Wall multiserver.o edutils.o calc.o stack.o -o run_multiserver

server.o: server/main.c
	gcc -o server.o -c server/main.c

multiserver.o: multiserver/main.c
	gcc -o multiserver.o -c multiserver/main.c

client.o: client/main.c
	gcc -o client.o -c client/main.c

stack.o: stack.c stack.h
	gcc -c stack.c

calc.o: calc.c calc.h
	gcc -c calc.c

edutils.o: edutils.c edutils.h
	gcc -c edutils.c

clean:
	rm *.o run_*

all: client server multiserver