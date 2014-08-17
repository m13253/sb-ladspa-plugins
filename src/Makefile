
CC=gcc
RM=rm -f
override CFLAGS+=-Wall -O3

.PHONY: all clean

all: sb_extrastereo.so

%.so: %.c
	$(CC) -fPIC -shared $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) *.so
