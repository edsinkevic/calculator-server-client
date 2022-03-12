main: main.o stack.o
	gcc main.o stack.o -o main 

main.o: main.c
	gcc -c main.c stack.c

stack.o: stack.c
	gcc -c stack.c

clean:
	rm *.o main
