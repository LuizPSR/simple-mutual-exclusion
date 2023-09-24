CC: gcc
GFLAGS: -g -Wall -pthread

all: ep1

ep1: ep1.o spend_time.o
	$(CC) $(GFLAGS) -o ep1 ep1.o spend_time.o

ep1.o:
	$(CC) $(GFLAGS) -c ep1.c spend_time.h

spend_time.o:
	$(CC) $(GFLAGS) -c spend_time.c

clean:
	rm -f ep1 ep1.o spend_time.o

test: ep1
	./ep1 < sample.txt

