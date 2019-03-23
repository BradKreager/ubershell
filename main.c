#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "helpers.h"
#include "globals.h"

void runshell(int prompt);

int main(int argc, char **argv)
{

	//run terminal
	if(argc == 1) {

		//debug begin
		if(DEBUG == "all" || DEBUG == "main") {
			printf("DEBUG: main()\n");
		}
		//debug end
		runshell(argc);

	}
	else if(argc == 2) {
		
		int script = open(argv[1], O_RDONLY, S_IRUSR | S_IWUSR);
		printf("argv[1]: %s", argv[1]);
		if(script == -1) {
			fwrite(error_message, strlen(error_message), 1, stderr);
			exit(1);
		}
		dup2(script, 0);
		runshell(argc);
		close(script);
		exit(0);
	}
	else {
		fwrite(error_message, strlen(error_message), 1, stderr);
		exit(1);
	}
}



void runshell(int prompt) {
	char *line = NULL,
	     *input = NULL,
	     *filename = NULL;
	size_t linesize = 0;
	ssize_t linelen;
	int rc[CMD_PARALLEL_MAX],
	rc_wait[CMD_PARALLEL_MAX];
	int redirect = 0;

	if(prompt == 1)
		fwrite("ubershell> ", 11, 1, stdout);
	while ((linelen = getline(&line, &linesize, stdin)) != -1) {
		size_t curr_thd = 0;
		struct filenames fs;
		//remove newline
		line = strsep(&line, "\n");

		if(line == '\0' || line == NULL || *line == 0) {
			if(prompt == 1)
				fwrite("\nubershell> ", 12, 1, stdout);
			continue;
		}

		//find redirection chars
		redirect = charCount(line, '>');

		//debug begin
		if(DEBUG == "all" || DEBUG == "main") {
			printf("redirect: %d\n", redirect);
		}
		//debug end

		if(redirect > 1) {
			//more than one redicrection token
			fwrite(error_message, strlen(error_message), 1, stderr);
			if(prompt == 1)
				fwrite("\nubershell> ", 12, 1, stdout);
			//skip rest of code and start over
			continue;
		}
		else if(redirect == 1) {
			input = strsep(&line, ">");
			fs = parseFilenames(line);
			if(fs.count != 1) {
				fwrite(error_message, strlen(error_message), 1, stderr);
				if(prompt == 1)
					fwrite("\nubershell> ", 12, 1, stdout);
				continue;
			}
		}
		else {
			input = line;
		}

		//debug begin
		if(DEBUG == "all" || DEBUG == "main") {
			printf("\nMain(): Calling parseParallelCmds()\n\n");
		}
		//debug end

		struct commands	cmds = parseParallelCmds(input);
		//			printf("cmds.count: %lu", cmds.count);

		//debug begin
		if(DEBUG == "all" || DEBUG == "main") {
			printf("\nMain(): Returned from parseParallelCmds()\n\n");
		}
		//debug end

		for(size_t i = 0; i < cmds.count && i < CMD_PARALLEL_MAX; i++) {

			if( (int) cmds.curr[i].builtin > -1 ) {
				executeBuiltin(cmds.curr[i]);
				continue;
			}

			// fork
			rc[i] = fork();

			if (rc[i] < 0) {
				fwrite(error_message, strlen(error_message), 1, stderr);
				err(1, "fork failed");
			}
			// if child, exec
			else if (rc[i] == 0) {
				//debug begin
				if(DEBUG == "all" || DEBUG == "main") {
					printf("cmds.curr[%lu].arg_count: %d\n", i, cmds.curr[i].arg_count);
					for(int db = 0; db < cmds.curr[i].arg_count; db++) {
						printf("argv[%d]: %s\n", db, cmds.curr[i].argv[db]);
					}
				}
				printf("\n");
				//debug end

				char *pathname = findExe(cmds.curr[i].argv[0]);

				if(redirect == 1) {
					int fd = open(fs.file[0], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
					dup2(fd, 1);
					dup2(fd, 2);
					close(fd);
				}
				execv(pathname, cmds.curr[i].argv);

				fwrite(error_message, strlen(error_message), 1, stderr);
				exit(1);
			}
		}
		//			if parent, wait
		//			for(size_t i = 0; i < cmds.count; i++) {
		//				if(cmds.curr[i].builtin == -1)
		//					rc_wait[i] = wait(NULL);
		//			}
		int rc_wt;
		while(rc_wt = wait(NULL) > 0);
		if(prompt == 1)
			fwrite("\nubershell> ", 12, 1, stdout);
		else
			fwrite("\n", 1, 1, stdout);
	}

	free(line);
	if (ferror(stdin))
		err(1, "getline");
}
