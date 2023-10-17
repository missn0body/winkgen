# Simply replace CC with your own compiler
# if it doesn't match the ones listed below.

#CC = i586-pc-msdosdjgpp-gcc
#CC = i686-w64-mingw32-gcc
#CC = x86_64-w64-mingw32-gcc
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
