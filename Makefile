# debugging:
# CFLAGS = -Og -g -pthread -Wall -Wno-unused -Wextra
# regular:
CFLAGS = -O2 -pthread -Wall -Wno-unused -Wextra

CONFIG_SRC = src/philosophers.h
MAIN_SRC = src/philosophers.c
HEADERS = src/common.h
INCLUDE_SRC = src/common.c

all: philosophers

philosophers: $(MAIN_SRC) $(INCLUDE_SRC) $(HEADERS) $(CONFIG_SRC) Makefile
	gcc -o $@ $(MAIN_SRC) $(INCLUDE_SRC) $(CFLAGS) -I $(HEADERS)

clean:
	rm -rf *~ *.o *.dSYM philosophers
