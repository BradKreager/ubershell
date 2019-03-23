#ifndef HELPERS_H
#define HELPERS_H

#include "globals.h"

int isBuiltin(char *cmd);
char *strcat_bounded(char *dest, char *src, size_t dest_max_len);
char *redirect(char *input);
char *findExe(char *exe);
int charCount(char *input, char target);
void addToPath(struct command c);
char nextChar(char *s);
char *wordsep(char **s);
size_t parseArgs(char *usr_cmd, char ***argv);
void stripBoundingWhiteSpace(char *s);
struct commands parseParallelCmds(char *usr_cmds);
void replaceChar(char *s, char old, char new, int preserveWithinQuotes);
void executeBuiltin(struct command c);
struct filenames parseFilenames(char *s);

#endif
