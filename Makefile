CC = cc
CFLAGS = -Wall -O2
LDFLAGS =

all: minmix
.c.o:
	$(CC) -c $(CFLAGS) $<
minmix: minmix.o
	$(CC) -o $@ $^ $(LDFLAGS)
clean:
	rm -f *.o minmix

