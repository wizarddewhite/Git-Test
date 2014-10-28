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

int init_client_sess(char *serv_name, struct sockaddr_in *serv_addr)
{
	if (client_session.type != SESS_NONE)
		return -RET_SESS_INIED;

	init_session(&client_session, serv_name, serv_addr, SESS_CLIENT);
	return RET_SESS_SUCCESS;
}

int deinit_client_sess()
{
	if (client_session.type != SESS_NONE)
		return -RET_SESS_UNINIED;

	deinit_session(&client_session);
	return RET_SESS_SUCCESS;
}
