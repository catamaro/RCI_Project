projeto: tcp_connection.o tcp_connect.o connect.h
	gcc $(CFLAGS) -o tcp_con tcp_connection.o tcp_connect.o -lm

tcp_connect.o: tcp_connect.c connect.h
	gcc $(CFLAGS) -c tcp_connect.c -lm

clean:
	rm -f *.o *.~ dkt *.gch