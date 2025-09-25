# Makefile  20-Oct-2011 Craig Milo Rogers <rogers@isi.edu>

# This is a very simple Makefile for the Maestro serial bootstrap code.

CFLAGS = -g -std=gnu99

all: config_mdb.o spd.o

config_mdb.o: config_mdb.c \
              config.h \
	      ../includes/sboot_types.h

spd.o: spd.c \
       spd.h \
       ../includes/sboot_types.h

clean:
	/bin/rm -f *.o
