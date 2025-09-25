# Makefile  20-Oct-2011 Craig Milo Rogers <rogers@isi.edu>

# This is a very simple Makefile for the Maestro serial bootstrap code.

CFLAGS = -g -std=gnu99

all: run_sboot create_sboot report_workspace_size convert_sboot_to_code

run_sboot: run_sboot.o read_sboot_program.o
	cc run_sboot.o read_sboot_program.o -o run_sboot

run_sboot.o: run_sboot.c \
	     run_sboot.h \
             read_sboot_program.h \
             ../execution/sboot_execution.c \
             ../execution/sboot_execution_info.c \
             ../includes/sboot_workspace.h \
             ../includes/sboot_types.h \
	     ../includes/sboot_wreg.h \
             ../includes/sboot_execution.h \
             ../includes/sboot_execution_info.h \
             ../includes/sboot_execution_state.h \
             ../includes/sboot_op.h \
             ../includes/sboot_program_info.h \
             ../includes/sboot_shout.h \
	     ../includes/sboot_features.h \
	     ../includes/sboot_enable_maestro_phase.h 

read_sboot_program.o: read_sboot_program.c \
                      read_sboot_program.h \
                      ../includes/sboot_types.h

create_sboot: create_sboot.o \
              ../encoding/sboot_encoding.o \
              ../mdb/config_mdb.o \
              ../mdb/spd.o
	cc create_sboot.o ../encoding/sboot_encoding.o ../mdb/config_mdb.o ../mdb/spd.o -o create_sboot

create_sboot.o: create_sboot.c \
	        create_sboot.h \
	        ../includes/sboot_encoding.h \
	        ../includes/sboot_types.h \
	        ../includes/sboot_wreg.h \
	        ../includes/sboot_op.h \
	        ../includes/sboot_enable_maestro_phase.h \
                ../mdb/boot.h \
                ../mdb/config.h \
                ../mdb/config_mdb.c \
                ../mdb/rshim.h \
                ../mdb/msh.h \
                ../mdb/spd.h \

report_workspace_size: report_workspace_size.o
	cc report_workspace_size.o -o report_workspace_size

report_workspace_size.o: report_workspace_size.c \
		         ../includes/sboot_workspace.h \
                         ../includes/sboot_types.h \
                         ../includes/sboot_wreg.h \
                         ../includes/sboot_execution_state.h \
                         ../includes/sboot_shout.h \
                         ../includes/sboot_features.h \
	                 ../includes/sboot_enable_maestro_phase.h

convert_sboot_to_code: convert_sboot_to_code.o read_sboot_program.o
	cc convert_sboot_to_code.o read_sboot_program.o -o convert_sboot_to_code

convert_sboot_to_code.o: convert_sboot_to_code.c \
             read_sboot_program.h \
             ../includes/sboot_types.h

clean:
	/bin/rm -f *.o run_sboot create_sboot report_workspace_size convert_sboot_to_code
