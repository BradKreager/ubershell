#include "globals.h"
#include "helpers.h"
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


char *strcat_bounded(char *dest, char *src, size_t dest_max_len) {

	char *result = NULL;
	size_t dest_len = strlen(dest),
	       src_len = strlen(src),
		   bytes_avail = dest_max_len - dest_len - 1;

	//debug begin
	if(DEBUG == "all" || DEBUG == "strcat_bounded") {
		printf("DEBUG: strcat_bounded()\n");
		printf("dest_size: %lu\n", dest_max_len);
		printf("dest_len: %lu\n", dest_len);
		printf("src_len: %lu\n", src_len);
		printf("String Out of Bounds: %s\n\n", ((dest_len + src_len + 1) > dest_max_len) ? "True" : "False");
	}
	//debug end

	result = strncat(dest, src, bytes_avail); 

	return result;

}






char *findExe(char *exe) {

	char *path_cpy = strdup(path);
	char *test_path = malloc(PATH_MAX_SIZE * sizeof(*test_path));

	if(!test_path || !path_cpy)
		return NULL;

	//maintain original str for future 
	//call to free()
	char *path_cpy_ptr = path_cpy;

	while(path_cpy_ptr != NULL) {
		memset(test_path, '\0', (PATH_MAX_SIZE * sizeof(*test_path)) );
		char *curr_tok = strsep(&path_cpy_ptr, ":");
		strcpy(test_path, curr_tok);

		if(curr_tok[strlen(curr_tok)-1] != '/') {
			char append_slash[2] = {'/','\0'};
			strcat(test_path, append_slash);
		}
		test_path = strcat_bounded(test_path, exe, PATH_MAX_SIZE);
		//debug begin
		if(DEBUG == "all" || DEBUG == "findExe") {
			printf("DEBUG: findExe()\n");
			printf("test_path: %s\n", test_path);
			printf("curr_tok: %s\n", curr_tok);
			printf("path_cpy_ptr: %s\n\n", path_cpy_ptr);
		}
		//debug end
		
		//Test if file exists at
		//current path
		int rval = access(test_path, R_OK);

		//if file exists at current path
		if(rval == 0) {
			free(path_cpy);
//			printf("path_cpy: %p\ntest_path: %p\n", path_cpy, test_path);
			return test_path;
		}
	}		
	free(test_path);
	free(path_cpy);
	return NULL;

}








char *redirect(char *input) {
	int len = strlen(input);
	char *ptr = input;
	
	for(int i = 0; i < len; i++) {
		if(*ptr == '>')
			return ptr;
		ptr++;
	}
	
	return NULL;

}







int charCount(char *input, char target) {
	int len = strlen(input),
		cnt = 0;
	
	for(int i = 0; i < len; i++) {
		if(input[i] == '>')
			cnt++;
	}
	return cnt;
}




void addToPath(struct command c) {
	char temp[2048] = "";
	for(int i = 1; i < c.arg_count; i++) {
		strcat_bounded(temp, c.argv[i], 2048);
		strcat_bounded(temp, ":", 2048);
	}

	strcpy(path, temp);
}







char nextChar(char *s) {
	return *(++s);
}







struct filenames parseFilenames(char *s) {
	struct filenames fstruct;

	fstruct.file = malloc(256 * sizeof(*fstruct.file));
	int i;
	for(i = 0; i < 256 && s != NULL; i++) {
		fstruct.file[i] = wordsep(&s);
	}
	fstruct.count = i;

	return fstruct;
}






char *wordsep(char **s) {
	int len = strlen(*s),
		inQuotes = 0,
		openQuote = 0;
	char *wordstart = *s,
		 *wordend = NULL,
		 *sref = *s;

	//debug begin
	if(DEBUG == "all" || DEBUG == "wordsep") {
		printf("DEBUG: wordsep()\n");
		printf("s: %p\n", s);
		printf("*s: %p\n", *s);
		printf("**s: %s\n", *s);
		printf("s[len]: %p\n", s[len]);
		printf("((*s)+len): %p\n", ((*s)+len));
		printf("&sref[len]: %p\n", &sref[len]);
		printf("wordstart: %p\n", wordstart);

	}
	//debug end
	
	if(wordstart == &sref[len]) {
		*s = NULL;
		return wordstart;
//		return NULL;
	}
	
	//debug begin
	if(DEBUG == "all" || DEBUG == "wordsep") {
		printf("Finding first character...\n");

	}
	//debug end

	while((*wordstart == ' ' || *wordstart == '\t') && wordstart != &sref[len]) {
		//debug begin
		if(DEBUG == "all" || DEBUG == "wordsep") {
			printf("current *wordstart: %p\n", wordstart);
		}
		//debug end
		wordstart++;
	}

	if(*wordstart == '"' || *wordstart == '\'')
		inQuotes = 1;

	wordend = wordstart;

	//debug begin
	if(DEBUG == "all" || DEBUG == "wordsep") {
		printf("final wordstart: %c\n", *wordstart);
		printf("final *wordstart: %p\n", wordstart);
		printf("inQuotes: %d\n", inQuotes);
		printf("wordend: %p\n", wordend);
		printf("*wordend: %c\n", *wordend);
		printf("Finding last character...\n");
	}
	//debug end

	if(inQuotes > 0) {
		if(wordend != &sref[len])
			wordend++;

		while((*wordend != '"' && *wordend != '\'') && wordend != &sref[len]) {

			//debug begin
			if(DEBUG == "all" || DEBUG == "wordsep") {
				printf("current *wordend: %c\n", *wordend);
			}
			//debug end
			
			if(*wordend == '\\' && nextChar(wordend) == '"')
				wordend++;

			wordend++;
		}
		if(*wordstart != *wordend) {
			openQuote = 1;
			wordend = wordstart;
		}
		else
			wordstart++;
	}

	if(openQuote > 0 || inQuotes < 1) {
		
		//debug begin
		if(DEBUG == "all" || DEBUG == "wordsep") {
			printf("openQuote: %d\n", openQuote);
		}
		//debug end
		
		while((*wordend != ' ' && *wordend != '\t') && wordend != &sref[len]) {

			//debug begin
			if(DEBUG == "all" || DEBUG == "wordsep") {
				printf("current *wordend: %c\n", *wordend);
			}
			//debug end
			wordend++;
		}
	}

	*wordend = '\0';

	//debug begin
	if(DEBUG == "all" || DEBUG == "wordsep") {
		printf("final wordend: %p\n", wordend);
		printf("final *wordend: %c\n", *wordend);
		printf("current *s: %p\n", *s);
	}
	//debug end

	if(wordend != &sref[len])
		*s = ++wordend;
	else
		*s = NULL;

	//debug begin
	if(DEBUG == "all" || DEBUG == "wordsep") {
		printf("final *s: %p\n", *s);
		printf("final **s: %s\n", *s);
		printf("wordstart: %p\n", wordstart);
		printf("\n");
	}
	//debug end

	return wordstart;

}






size_t parseArgs(char *usr_cmd, char ***argv) {

	char **args = malloc(CMD_ARGUMENTS_MAX * sizeof(*args));
	size_t arg_cnt = 0;

	//remove newline char
	usr_cmd = strsep(&usr_cmd, "\n");

	//debug begin
	if(DEBUG == "all" || DEBUG == "parseArgs") {
		printf("DEBUG: parseArgs()\n");
	}
	//debug end

	//parse words at spaces
	for(arg_cnt = 0; usr_cmd != NULL && arg_cnt < CMD_ARGUMENTS_MAX; arg_cnt++) {

		//debug begin
		if(DEBUG == "all" || DEBUG == "parseArgs") {
			printf("Calling wordsep()...\n\n");
		}
		//debug end

		args[arg_cnt] = wordsep(&usr_cmd);


		//debug begin
		if(DEBUG == "all" || DEBUG == "parseArgs") {
			printf("Returned from wordsep()\n");
		}
		//debug end


		//debug begin
		if(DEBUG == "all" || DEBUG == "parseArgs") {
			printf("args[%lu]: %s\n", arg_cnt, args[arg_cnt]);
		}
		//debug end
	}

	//Create NULL pointer as last element of argv
	//if previous element returned from wordsep()
	//is empty string is an empty string replace
	//with NULL, otherwise create new element.
	if(arg_cnt > 0) {
		if(strcmp(args[arg_cnt - 1], "") == 0) {
			args[arg_cnt - 1] = NULL;
			arg_cnt--;
		}
		else
			args[arg_cnt] = NULL;
	}

	//debug begin
	if(DEBUG == "all" || DEBUG == "parseArgs") {
		printf("arg_cnt: %lu\n", arg_cnt);
		printf("\n");
	}
	//debug end

	//assign argument array 
	//to array passed by reference 
	*argv = args;

	//return arg_cnt + 1 for easy
	//use of return value in loops
	return (arg_cnt);

}






void stripBoundingWhiteSpace(char *s) {
	int len = strlen(s);

	//debug begin
	if(DEBUG == "all" || DEBUG == "stripBoundingWhiteSpace") {
		printf("DEBUG: stripBoundingWhiteSpace()\n");
		printf("len: %d\n", len);
		printf("str: %s\n", s);
	}
	//debug end

	for(int i = len-1; s[i] == ' ' || s[i] == '\t'; --i) {
		s[i] = '\0';
	}

	if(s[0] == ' ' || s[0] == '\t') {
		char *ptr_first_char = NULL;
		int char_pos = 0;
		for(char_pos; s[char_pos] == ' ' || s[char_pos] == '\t'; ++char_pos) {
			ptr_first_char = &s[char_pos + 1];
		}

		for(int i = 0; i < len || ptr_first_char == &s[len]; i++) {

			//debug begin
			if(DEBUG == "all" || DEBUG == "stripBoundingWhiteSpace") {
				printf("ptr_first_char: %c\ns[%d]: %c\n",
						*ptr_first_char, i, s[i]);
			}
			//debug end

			s[i] = *ptr_first_char++;
		}

		//debug begin
		if(DEBUG == "all" || DEBUG == "stripBoundingWhiteSpace") {
			printf("final str: %s\n\n", s);
		}
		//debug end
	}
}






struct commands parseParallelCmds(char *usr_cmds) {
	struct commands cmds;

	cmds.curr = malloc(CMD_PARALLEL_MAX * sizeof(struct command));

	//remove newline char
	usr_cmds = strsep(&usr_cmds, "\n");

	//debug begin
	if(DEBUG == "all" || DEBUG == "parseParallelCmds") {
		printf("DEBUG: parseParallelCmds()\n");
		printf("usr_cmds: %s\n", usr_cmds);
	}
	//debug end

	char * curr_cmd;
	size_t cmd_cnt;

	//parse cmds at & 
	for(cmd_cnt = 0; (usr_cmds != NULL) && (cmd_cnt < CMD_PARALLEL_MAX); cmd_cnt++) {
		curr_cmd = strsep(&usr_cmds, "&");

		//debug begin
		if(DEBUG == "all" || DEBUG == "parseParallelCmds") {
			printf("curr_cmd: %s\n", curr_cmd);
			printf("Calling parseArgs()...\n\n");
		}
		//debug end
		cmds.curr[cmd_cnt].arg_count = parseArgs(curr_cmd, &cmds.curr[cmd_cnt].argv);

		//debug begin
		if(DEBUG == "all" || DEBUG == "parseParallelCmds") {
			printf("\nReturned from parseArgs()\n\n");
			printf("Calling isBuiltin()...\n\n");
		}
		//debug end
		
		cmds.curr[cmd_cnt].builtin = (builtin)isBuiltin(cmds.curr[cmd_cnt].argv[0]);

		//debug begin
		if(DEBUG == "all" || DEBUG == "parseParallelCmds") {
			printf("\nReturned from isBuiltin()");
			int b = cmds.curr[cmd_cnt].builtin;
			printf("builtin: %s", 
					(b == 2) ?
					"quit" :
					(b == 1) ?
					"cd" :
					(b == 0) ?
					"pathmod" : 
					"not builtin");
			printf("\n");
			printf("cmd_cnt: %lu\n", cmd_cnt + 1);
		}
		//debug end
	}
	cmds.count = cmd_cnt;

	return cmds;

}






void replaceChar(char *s, char old, char new, int preserveWithinQuotes) {
	char *ptr = &s[0];
	int len = strlen(s);
	int inQuotes = 0;

	//debug begin
	if(DEBUG == "all" || DEBUG == "replaceChar") {
		printf("DEBUG: replaceChar()\n");
		printf("s: %s\n", s);
		printf("len: %d\n", len);
		printf("old: %c\nnew: %c\n", old, new);
		printf("&s[len]: %p\n", &s[len]);
		printf("preserveWithinQuotes: %d\n"
				, preserveWithinQuotes);
	}
	//debug end

	while(ptr != &s[len]) {

		if(preserveWithinQuotes > 0)
			if((*ptr == '"' || *ptr == '\'') && (inQuotes < 1))
				inQuotes = 1;
			else if((*ptr == '"' || *ptr == '\'') 
					&& (inQuotes > 0)
					&& *(ptr - 1) != '\\' )
				inQuotes = 0;

		if(*ptr == old && inQuotes < 1)
			*ptr = new;

		//debug begin
		if(DEBUG == "all" || DEBUG == "replaceChar") {
			printf("&ptr: %p\n", ptr);
			printf("*ptr: %c\n", *ptr);
			printf("*(ptr - 1): %c\n", *(ptr - 1));
		}
		//debug end

		ptr++;
	}

}






int isBuiltin(char *cmd) {
	if(cmd == "" || cmd == NULL)
		return -1;
	for(int i = 0; i < 3; i++) {
		if(strcmp(cmd, builtins[i]) == 0)
			return i;
	}
	return -1;
}







void executeBuiltin(struct command c, struct commands *cmds, struct filenames *fns) {
	//debug begin
	if(DEBUG == "all" || DEBUG == "executeBuiltin") {
		printf("DEBUG: executeBuiltin()\n");
	}
	//debug end
	switch (c.builtin){
		case pathmod:
			if(c.arg_count-1 > 1) {
				fwrite(error_message, strlen(error_message), 1, stderr);
			}
			else {
				addToPath(c);
			}
			break;
		case cd:
			if(c.arg_count-1 != 1) {
				fwrite(error_message, strlen(error_message), 1, stderr);
			}
			else {
				if(chdir(c.argv[1]) < 0)
					fwrite(error_message, strlen(error_message), 1, stderr);
				else {
					//debug begin
					if(DEBUG == "all" || DEBUG == "replaceChar") {
						printf("chdir succeeded\n");
					}
					//debug end
					pwd();
				}

			}
			break;
		case quit:
			if(c.arg_count-1 > 0) {
				fwrite(error_message, strlen(error_message), 1, stderr);
			}
			else { 
//				freeCommands(cmds);
//				freeFilenames(fns);
				exit(0);
			}
			break;
		default:
			fwrite(error_message, strlen(error_message), 1, stderr);
			break;
	}
	//debug begin
	if(DEBUG == "all" || DEBUG == "replaceChar") {
		printf("\n");
	}
	//debug end
}






void pwd(void) {
	char *buf;
	int i;	

	for(i = 1; i < 10; i++) {
		buf = malloc(PATH_MAX_SIZE * i * sizeof(*buf));
		if(getcwd(buf, PATH_MAX_SIZE * i) == NULL) {
			if(errno == ENAMETOOLONG) {
				free(buf);
			}

		}
		else
			break;
	}
	if(i == 10 && errno == ENAMETOOLONG) {
		fwrite(error_message, strlen(error_message), 1, stderr);
		return;
	}
	fwrite(buf, strlen(buf), 1, stdout);
	free(buf);
}






void freeCommands(struct commands *c) {
	for(size_t i = 0; i < c->count; i++) {
		free(c->curr[i].argv);
	}
	free(c->curr);
}





void freeFilenames(struct filenames *fns) {
		free(fns->file);
}
