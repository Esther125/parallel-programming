.SILENT:
CC = gcc
CFLAGS = -O3 -Ofast -pthread -std=c11 -fno-math-errno -fno-trapping-math -funroll-loops -mtune=native

all: pi.out

pi.out: pi.o
	$(CC) $(CFLAGS) pi.o -o pi.out
pi.o: pi.c
	$(CC) $(CFLAGS) -c pi.c
clean:
	$(RM) *.o *.out
