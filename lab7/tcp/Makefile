CC = gcc
CFLAGS = -Wall -Wextra -std=c99

all: tcpserver tcpclient

tcpserver: tcpserver.c
	$(CC) $(CFLAGS) -o tcpserver tcpserver.c

tcpclient: tcpclient.c
	$(CC) $(CFLAGS) -o tcpclient tcpclient.c

clean:
	rm -f tcpserver tcpclient

.PHONY: all clean
