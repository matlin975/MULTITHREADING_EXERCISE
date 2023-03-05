EXE=mfind
CC=gcc

CFLAGS=-std=gnu11 -Wall -Wextra -Werror -Wmissing-declarations \
	-Wmissing-prototypes -Werror-implicit-function-declaration \
	-Wreturn-type -Wparentheses -Wunused -Wold-style-definition\
	-Wundef -Wshadow -Wstrict-prototypes -Wswitch-default\
	-Wunreachable-code -g

LDFLAGS = -pthread

all:	mfind

mfind:	mfind.o list.o
	$(CC) $(LDFLAGS) -o $(EXE) mfind.o list.o

mfind.o:	mfind.c mfind.h
	$(CC) $(CFLAGS) -c mfind.c

list.o:	list.c list.h
	$(CC) $(CFLAGS) -c list.c

.PHONY:	clean
clean:
	clear
	rm mfind mfind.o
