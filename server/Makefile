main: main.o stack.o calc.o
	gcc main.o stack.o calc.o -o main 

main.o: main.c
	gcc -c main.c

stack.o: stack.c
	gcc -c stack.c

calc.o: calc.c
	gcc -c calc.c

clean:
	rm *.o main
