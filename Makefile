client: ./client/* ./common/*
	cd client && \
	clang -o client.o client.c client_utils.c ../common/colored_string.c ../common/language_server.c && \
	echo "Recompiled client" && \
	./client.o

server: ./server/* ./common/*
	cd server && \
	clang -o server.o server.c server_utils.c ../common/colored_string.c ../common/language_server.c && \
	echo "Recompiled server" && \
	./server.o
smalltest: test.c
	clang -o test.o test.c
	./test.o
test2: test.c test2.c test2.h
	clang -o test.o test.c test2.c
	./test.o
colortest: test.c common/*
	clang -o test.o test.c ./common/colored_string.c
	./test.o
lstest: test.c common/*
	clang -o test.o test.c ./common/colored_string.c ./common/language_server.c
	./test.o