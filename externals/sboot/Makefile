# Makefile  20-Oct-2011 Craig Milo Rogers <rogers@isi.edu>

# This is a very simple Makefile for the Maestro serial bootstrap code.

CFLAGS = -g -std=gnu99

all:
	cd mdb       && make all
	cd encoding  && make all
	cd tools     && make all
	cd ccsc_test && make all

clean:
	cd mdb       && make clean
	cd encoding  && make clean
	cd tools     && make clean
	cd ccsc_test && make clean
