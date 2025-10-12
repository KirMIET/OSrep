CC=gcc
CFLAGS=-I.

parallel_sum: utils.o utils.h sum.o sum.h
	$(CC) -o parallel_sum utils.o sum.o parallel_sum.c $(CFLAGS)

utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

sum.o : sum.h
	$(CC) -o sum.o -c sum.c $(CFLAGS)

clean :
	rm -f utils.o sum.o