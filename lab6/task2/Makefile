CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -pthread

SRC_COMMON = common.c
OBJ_COMMON = $(SRC_COMMON:.c=.o)

all: client server

client: client.o $(OBJ_COMMON)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

server: server.o $(OBJ_COMMON)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o client server
