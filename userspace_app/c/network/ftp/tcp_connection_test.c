/*
 * =====================================================================================
 *
 *       Filename:  tcp_connection_test.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/26/2014 09:34:56 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include "session_client.h"
#include "protocol.h"
#include "display.h"
#include "tcp_connection_test.h"

int tcp_test(char *addr, int port)
{
	struct session *sess;
	char buff[1024];

	init_client_sess(addr, port, NULL);
	sess = get_client_sess();

	while (1) {
		get_command("tcp_test", buff, sizeof(buff));
	}
}
