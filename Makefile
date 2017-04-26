CC=gcc
CFLAGS=-g
DEPS = includes.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: oss userProcess

oss: parent.o system.o
	gcc -o $@ $^ $(CFLAGS)

userProcess: child.o system.o
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o oss userProcess a.out
