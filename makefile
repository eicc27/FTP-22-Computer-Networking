client: client.c command.* util.*
	gcc -I${HOME}/incl -c client.c command.c util.c -std=c99
	gcc -o client client.o command.o util.o
	rm -f client.o command.o util.o
	./client