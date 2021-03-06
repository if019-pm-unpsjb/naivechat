CC=gcc
CFLAGS=-Wall -Werror -g -pthread 

PROG=client minclient

.PHONY: all
all: $(PROG)

%: %.c 
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(PROG)
