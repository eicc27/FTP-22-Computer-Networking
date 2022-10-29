client: client.c
	gcc -I${HOME}/incl -c client.c
	gcc -o client.exe client.o
	rm -f client.o
	./client.exe