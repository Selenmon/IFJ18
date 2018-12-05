PRJ=ifj18
#
PROGS=$(PRJ)
FILES=main.c symtable.c dynamic_string.c scanner.c stackgenerator.c expression.c codegenerator.c parser.c
CC=gcc
CFLAGS=-Wall -W -Wextra -g -std=c99 -pedantic

all: $(PROGS)

$(PRJ):	$(FILES) error.h
	$(CC) $(CFLAGS) -o $@ $(FILES) -lm

clean:
	rm -f *.o *.out $(PROGS)
