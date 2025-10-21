CC = gcc
CFLAGS = -Wall -Wextra -std=c99

all: udpserver udpclient

udpserver: udpserver.c
	$(CC) $(CFLAGS) -o udpserver udpserver.c

udpclient: udpclient.c
	$(CC) $(CFLAGS) -o udpclient udpclient.c

clean:
	rm -f udpserver udpclient

.PHONY: all clean
