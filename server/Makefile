main: main.o stack.o calc.o edutils.o
	gcc main.o stack.o calc.o edutils.o -o main 

main.o: main.c
	gcc -c main.c

stack.o: stack.c
	gcc -c stack.c

calc.o: calc.c edutils.o
	gcc -c edutils.o calc.c

edutils.o: edutils.c
	gcc -c edutils.c

clean:
	rm *.o main
