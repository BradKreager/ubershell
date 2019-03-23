#ifndef GLOBALS_H
#define GLOBALS_H

#define CMD_ARGUMENTS_MAX 16
#define CMD_PARALLEL_MAX 32
#define PATH_MAX_SIZE 1024
#define ARGUMENTS_STRLEN_MAX 256
#ifndef DEBUG
#define DEBUG "all"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>


typedef enum {
	pathmod = 0,
	cd = 1,
	quit = 2,
	none = -1
} builtin;


struct command {
	int arg_count;
	builtin builtin;
	char **argv;
};


struct commands {
	size_t count;
	struct command *curr;
};


struct filenames {
	size_t count;
	char **file;
};


extern char path[2048];

extern char *builtins[3];

extern char error_message[30];

#endif
