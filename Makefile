CFLAGS = -Og -g -pthread -Wall -Wno-unused
PHIL_SRC = src/philosophers.c

all: philosophers

philosophers: $(PHIL_SRC) Makefile
	gcc -o $@ $(CFLAGS) $(PHIL_SRC)

clean:
	rm -rf *~ *.o philosophers *.dSYM
