
DESTDIR=
PREFIX=$(DESTDIR)/usr/local
LADSPADIR=$(PREFIX)/ladspa

CC=gcc
RM=rm -f
MKDIR=mkdir -p
INSTALL=install
override CFLAGS+=-Wall -O3
override LDFLAGS+=-lm

.PHONY: all install clean

all: sb_extrastereo.so sb_earwax.so

%.so: %.c
	$(CC) -fPIC -shared $(CFLAGS) -o $@ $^ $(LDFLAGS)

install:
	$(MKDIR) $(LADSPADIR)/
	$(INSTALL) *.so $(LADSPADIR)/

clean:
	$(RM) *.so
