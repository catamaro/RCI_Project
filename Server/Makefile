CFLAGS= -Wall -O3 -pedantic

projeto: main.o tcp_connect.o operations.o functions.o connect.h
	gcc $(CFLAGS) -o dkt main.o tcp_connect.o operations.o functions.o -lm

tcp_connect.o: tcp_connect.c connect.h
	gcc $(CFLAGS) -c tcp_connect.c -lm

operations.o: operations.c connect.h
	gcc $(CFLAGS) -c operations.c -lm

functions.o: functions.c connect.h
	gcc $(CFLAGS) -c functions.c -lm

clean:
	rm -f *.o *.~ dkt *.gch