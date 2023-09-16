CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -Os -g

winkgen.o: winkgen.c
	$(CC) -c $^ -o $@ $(CFLAGS)

winkgen: winkgen.o
	$(CC) $^ -o $@ $(CFLAGS)

.PHONY: all clean

all: winkgen

clean:
	rm -f *.o winkgen
