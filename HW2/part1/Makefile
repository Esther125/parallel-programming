.SILENT:
CC = gcc
CFLAGS = -O3 -Ofast -march=native -mavx2 -mfma -funroll-loops -fno-math-errno -pipe -pthread -std=c11

all: pi.out
serial: pi-serial.out

pi.out: pi.o
	$(CC) $(CFLAGS) pi.o -o pi.out
pi.o: pi.c
	$(CC) $(CFLAGS) -c pi.c

# Serial Version
pi-serial.out: pi-serial.o
	$(CC) $(CFLAGS) pi-serial.o -o pi-serial.out
pi-serial.o: pi-serial.c
	$(CC) $(CFLAGS) -c pi-serial.c

clean:
	$(RM) *.o *.out
