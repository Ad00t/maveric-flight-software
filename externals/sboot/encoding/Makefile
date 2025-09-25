# Makefile  20-Oct-2011 Craig Milo Rogers <rogers@isi.edu>

# This is a very simple Makefile for the Maestro serial bootstrap code.

CFLAGS = -g -std=gnu99

all: sboot_encoding.o

sboot_encoding.o: sboot_encoding.c \
                  ../includes/sboot_encoding.h \
                  ../includes/sboot_program.h \
                  ../includes/sboot_types.h \
	          ../includes/sboot_wreg.h \
                  ../includes/sboot_op.h \
                  ../includes/sboot_features.h \
                  ../includes/sboot_enable_maestro_phase.h \
                  ../mdb/rshim.h

clean:
	/bin/rm -f *.o
