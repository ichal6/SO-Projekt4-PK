main: *.c
	gcc -pthread klient.c -o klient
	gcc serwer.c -o serwer
