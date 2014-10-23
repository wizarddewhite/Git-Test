/*
 * =====================================================================================
 *
 *       Filename:  command.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/23/2014 11:30:40 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <string.h>
#include "command.h"
#include "display.h"

static int open(char *host)
{
	return 0;
}

static int quit(char *host)
{
	return -RET_QUIT;
}

static struct command commands[] = {
	{"quit", quit},
	{"open", open},
	{NULL, NULL},
};

int handle_command(char *raw)
{
	struct command *cmd;

	for (cmd = commands; cmd->name; cmd++) {
		if (!strncmp(cmd->name, raw, strlen(cmd->name)))
			return cmd->handle(raw);
	}

	return -RET_NOCMD;
}
