OBJ = $(CURDIR)/obj
DEPS = $(CURDIR)/deps

client: $(OBJ)/client.o $(OBJ)/edutils.o
	gcc -Wall $(OBJ)/client.o $(OBJ)/edutils.o -o run_client

server: $(OBJ)/server.o $(OBJ)/edutils.o $(OBJ)/calc.o $(OBJ)/stack.o
	gcc -Wall $(OBJ)/server.o $(OBJ)/edutils.o $(OBJ)/calc.o $(OBJ)/stack.o -o run_server

multiserver: $(OBJ)/multiserver.o $(OBJ)/edutils.o $(OBJ)/calc.o $(OBJ)/stack.o
	gcc -Wall $(OBJ)/multiserver.o $(OBJ)/edutils.o $(OBJ)/calc.o $(OBJ)/stack.o -o run_multiserver

$(OBJ)/server.o: server/main.c $(OBJ)
	gcc -o $(OBJ)/server.o -c server/main.c

$(OBJ)/multiserver.o: multiserver/main.c $(OBJ)
	gcc -o $(OBJ)/multiserver.o -c multiserver/main.c

$(OBJ)/client.o: client/main.c $(OBJ)
	gcc -o $(OBJ)/client.o -c client/main.c

$(OBJ)/stack.o: $(DEPS)/stack.c $(DEPS)/stack.h $(OBJ)
	gcc -o $(OBJ)/stack.o -c $(DEPS)/stack.c

$(OBJ)/calc.o: $(DEPS)/calc.c $(DEPS)/calc.h $(OBJ)
	gcc -o $(OBJ)/calc.o -c $(DEPS)/calc.c

$(OBJ)/edutils.o: $(DEPS)/edutils.c $(DEPS)/edutils.h $(OBJ)
	gcc -o $(OBJ)/edutils.o -c $(DEPS)/edutils.c

$(OBJ):
	mkdir obj && mkdir deps

clean:
	rm *.o obj/*.o run_*

build: client server multiserver