CC = gcc
OBJ = $(CURDIR)/obj
DEPS = $(CURDIR)/deps
CLIENTDEPS = $(OBJ)/client.o $(OBJ)/edutils.o
SERVERDEPS = $(OBJ)/server.o $(OBJ)/edutils.o $(OBJ)/calc.o $(OBJ)/stack.o
MULTISERVERDEPS = $(OBJ)/multiserver.o $(OBJ)/edutils.o $(OBJ)/calc.o $(OBJ)/stack.o

client: $(CLIENTDEPS)
	$(CC) -Wall $(CLIENTDEPS) -o run_$@

server: $(SERVERDEPS)
	$(CC) -Wall $(SERVERDEPS) -o run_$@

multiserver: $(MULTISERVERDEPS) 
	$(CC) -Wall $(MULTISERVERDEPS) -o run_$@

$(OBJ)/%.o: %/main.c $(OBJ)
	$(CC) -o $@ -c $<

$(OBJ)/%.o: $(DEPS)/%.c $(DEPS)/%.h $(OBJ)
	$(CC) -o $@ -c $<

$(OBJ):
	mkdir $(OBJ) && mkdir $(DEPS)

clean:
	rm obj/*.o run_*

build: client server multiserver