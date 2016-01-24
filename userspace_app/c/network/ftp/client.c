/*
 * =====================================================================================
 *
 *       Filename:  client.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/22/2014 08:30:11 PM
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

#include "display.h"
#include "command.h"

char command[64];

int main()
{
	int ret;
	screen_init(1);
	while (1) {
		get_command("ftp", command, sizeof(command));
		ret = handle_command(command);
		if (ret == -RET_QUIT)
			break;
		if (ret == -RET_NOCMD)
			print_help();
	}
	screen_dest();
	return 0;
}
