.SILENT:
CC = gcc
CFLAGS = -O2 -Wall -pthread

all: pi.out
pi.out: pi.o
	$(CC) $(CFLAGS) pi.o -o pi.out
pi.o: pi.c
	$(CC) $(CFLAGS) -c pi.c
clean:
	$(RM) *.o pi.out