
CC=gcc
RM=rm -f
override CFLAGS+=-Wall -O3

.PHONY: all clean

all: sb_extrastereo.so

.c.so:
	$(CC) -fPIC -shared $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) *.so
