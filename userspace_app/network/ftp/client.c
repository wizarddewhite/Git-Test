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

char command[64];

int main()
{
	screen_init();
	while (1) {
		get_command(command, sizeof(command));
		if (strncmp("quit", command, strlen("quit")) == 0)
			break;
	}
	screen_dest();
	return 0;
}
