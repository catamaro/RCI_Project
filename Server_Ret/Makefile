CFLAGS= -Wall -O3 -pedantic

projeto: main.o connections.o operations.o functions.o main.h
	gcc $(CFLAGS) -o dkt main.o connections.o operations.o functions.o -lm

connections.o: connections.c main.h
	gcc $(CFLAGS) -c connections.c -lm

operations.o: operations.c main.h
	gcc $(CFLAGS) -c operations.c -lm

functions.o: functions.c main.h
	gcc $(CFLAGS) -c functions.c -lm

clean:
	rm -f *.o *.~ dkt *.gch