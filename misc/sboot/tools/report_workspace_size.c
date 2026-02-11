// report_workspace_size.c  28-Oct-2011 Craig Milo Rogers <rogers@isi.edu>

#include <stdio.h>

#include "../includes/sboot_workspace.h"

int main(argc, argv)
int argc;                               /* Number of arguments. */
char **argv;                            /* Argument array. */
{
  printf("sizeof(sboot_workspace_t)=%d bytes.\n", sizeof(sboot_workspace_t));
}
