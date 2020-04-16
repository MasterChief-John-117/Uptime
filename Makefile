CC = gcc
CFLAGS  = -Wall -Wextra -Werror -pedantic -pthread -O2

default: pre server

pre:
	mkdir -p ./bin

server: pre src/server.c
	$(CC) $(CFLAGS) -o ./bin/server ./src/server.c

clean:
	rm -rf ./bin

