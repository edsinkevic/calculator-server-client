CC = gcc
OBJ = $(CURDIR)/obj
DEPS = $(CURDIR)/src/deps
SRC = $(CURDIR)/src
BIN = $(CURDIR)/bin
CLIENTDEPS = $(OBJ)/client.o $(OBJ)/edutils.o
SERVERDEPS = $(OBJ)/server.o $(OBJ)/edutils.o $(OBJ)/calc.o $(OBJ)/stack.o
MULTISERVERDEPS = $(OBJ)/multiserver.o $(OBJ)/edutils.o $(OBJ)/calc.o $(OBJ)/stack.o

client: $(CLIENTDEPS)
	$(CC) -Wall $(CLIENTDEPS) -o $(BIN)/$@

server: $(SERVERDEPS)
	$(CC) -Wall $(SERVERDEPS) -o $(BIN)/$@

multiserver: $(MULTISERVERDEPS) 
	$(CC) -Wall $(MULTISERVERDEPS) -o $(BIN)/$@

$(OBJ)/%.o: $(SRC)/%/main.c $(OBJ)
	$(CC) -o $@ -c $<

$(OBJ)/%.o: $(DEPS)/%.c $(DEPS)/%.h $(OBJ)
	$(CC) -o $@ -c $<

$(OBJ):
	mkdir $(OBJ) $(BIN)

clean:
	rm -rf obj bin

build: client server multiserver