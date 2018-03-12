run: main
	./main ../../samples/test*
fulltest: main
	./main ../../samples/*

main: main.o ../dimacs/dimacs_reader.o
	gcc -O3 -o main main.o ../dimacs/dimacs_reader.o

main.o: main.c ../../inc/dimacs_reader.h
	gcc -O3 -c main.c -I ../../inc

clean:
	rm -f main *.o
