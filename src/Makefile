
CC=gcc
RM=rm -f
override CFLAGS+=-Wall -O3
override LDFLAGS+=-lm

.PHONY: all clean

all: sb_extrastereo.so sb_earwax.so

%.so: %.c
	$(CC) -fPIC -shared $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) *.so
