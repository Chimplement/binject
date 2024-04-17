#include <stdio.h>
#include <stdlib.h>

#include "ansi.h"

void exit_error(char* error)
{
	fprintf(stderr, BOLD RED"Error:"RESET_BOLD WHITE" %s\n", error);
	exit(1);
}

void help(char* program_name)
{
	printf("Expected:\n");
	printf("%s "UNDERLINE"TARGET"RESET_UNDERLINE" "UNDERLINE"PAYLOAD"RESET_UNDERLINE"\n", program_name);
}