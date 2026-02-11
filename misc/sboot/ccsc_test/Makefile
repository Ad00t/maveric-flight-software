# Makefile  20-Oct-2011 Craig Milo Rogers <rogers@isi.edu>

# This is a very simple Makefile for the Maestro serial bootstrap code.

CFLAGS = -g -std=gnu99

all: ccsc_test

ccsc_test: ccsc_test.c \
           ../execution/sboot_execution.c \
           ../includes/sboot_workspace.h \
           ../includes/sboot_types.h \
           ../includes/sboot_wreg.h \
           ../includes/sboot_execution.h \
           ../includes/sboot_execution_state.h \
           ../includes/sboot_op.h \
           ../includes/sboot_shout.h \
           ../includes/sboot_features.h \
           ../includes/sboot_enable_maestro_phase.h
	ccsc +FD ccsc_test.c

clean:
	/bin/rm -f *.cod *.err *.esym *.hex *.lst *.sym
