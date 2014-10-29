/*
 * =====================================================================================
 *
 *       Filename:  session_client.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/25/2014 06:03:01 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>

#include "session.h"

static struct session client_session;

int init_client_sess(char *serv_addr, int port)
{
	int ret;

	/* If it has been initialized, just return */
	if (client_session.state != SESS_UNINITED)
		return -1;

	ret = init_session(&client_session, SESS_CLIENT, serv_addr, port);
	if (ret)
		return -1;

	return 0;
}

int deinit_client_sess()
{
	/* If it has not initialized, just return */
	if (client_session.state != SESS_INITED)
		return -1;

	deinit_session(&client_session);
	return 0;
}
