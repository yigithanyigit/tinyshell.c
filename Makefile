CC=clang -g
CCFLAGS= -Wall -O0 -fsanitize=address
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)

all: clean prog run

prog: $(OBJECTS)
	$(CC) $(CCFLAGS) $(OBJECTS) -o prog.out

run:
	./prog.out

clean:
	rm -rf *.o
	rm -rf prog.out
