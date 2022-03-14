main: main.o edutils.o
	gcc -Wall edutils.o main.o -o main 

main.o: main.c
	gcc -c main.c

edutils.o: ../edutils.c
	gcc -c ../edutils.c

clean:
	rm *.o main
