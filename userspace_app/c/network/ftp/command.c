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
#include <stdlib.h>
#include "command.h"
#include "display.h"
#include "tcp_connection_test.h"

static struct command commands[];

static int open(struct command *comm, char *command)
{
	return RET_SUSS;
}

static int quit(struct command *comm, char *command)
{
	return -RET_QUIT;
}

static int help(struct command *comm, char *command)
{
	char *index;
	struct command *cmd;

	index = command + strlen("help");

	while (*index == ' ')
		index++;

	if (*index == '\n') {
		print_message(comm->help);
		prompt("ftp");
		return RET_SUSS;
	}

	for (cmd = commands; cmd->name; cmd++) {
		if (!strncmp(index, cmd->name, strlen(cmd->name))) {
			print_message(cmd->help);
			break;
		}
	}

	if (!cmd->name) {
		print_vmessage("Not a valid command: %s\n", index);
		print_message("The commands we support now:\n");
		for (cmd = commands; cmd->name; cmd++)
			print_vmessage("\t%s\n", cmd->name);
	}
	prompt("ftp");

	return RET_SUSS;
}

static int tcp_client(struct command *comm, char *command)
{
	char *start, *end;
	char addr[16];
	int port;

	start = command + 3;

	while (*start == ' ')
		start++;

	if (*start == '\n')
		return 0;

	end = strchr(start, ' ');
	strncpy(addr, start, end - start);

	start = end;

	while (*start == ' ')
		start++;
	end = strchr(start, ' ');

	port = atoi(start);
	return 0;
}

static struct command commands[] = {
	{"quit", quit,
	 "Exit the program\n"
	},
	{"open", open,
	 "Open a ftp connection:\n"
	 "\topen host_addr\n"
	},
	{"help", help,
	 "Print help message of a valid command:\n"
	 "\thelp\n"
	 "\thelp command_name\n"
	},
	{"tcp", tcp_client,
	 "tcp client test\n"
	},
	{NULL, NULL}, /* must be the last one */
};

int handle_command(char *raw)
{
	struct command *cmd;

	for (cmd = commands; cmd->name; cmd++) {
		if (!strncmp(cmd->name, raw, strlen(cmd->name)))
			return cmd->handle(cmd, raw);
	}

	return -RET_NOCMD;
}