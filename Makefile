CFLAGS = -Og -g -pthread -Wall -Wno-unused -Wextra

MAIN_SRC = src/philosophers.c
HEADERS = src/common.h
INCLUDE_SRC = src/common.c

all: philosophers

philosophers: $(MAIN_SRC) $(INCLUDE_SRC) $(HEADERS) Makefile
	gcc -o $@ $(CFLAGS) $(MAIN_SRC) $(INCLUDE_SRC) -I $(HEADERS)

clean:
	rm -rf *~ *.o *.dSYM philosophers
