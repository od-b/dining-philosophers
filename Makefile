CFLAGS = -Og -Wall -pthread
PHIL_SRC = philosophers.c

all: philosophers

philosophers: $(PHIL_SRC) Makefile
	gcc -o $@ $(CFLAGS) $(PHIL_SRC)

clean:
	rm -rf *~ *.o philosophers *.dSYM
