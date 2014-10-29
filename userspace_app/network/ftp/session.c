/*
 * =====================================================================================
 *
 *       Filename:  session.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/24/2014 11:09:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "session.h"

/* 
 * return 0 on success
 */
int init_session(struct session *sess, enum session_type type,
		char *addr, int port)
{
	int ret;
	sess->type = type;

	/* Check the statuts. If it is already used, skip it. */
	if (sess->state == SESS_INITED)
		return -1;

	/* Create the connection */
	if (type == SESS_CLIENT)
		ret = create_client_conn(&sess->conn, addr, port);
	else if (type == SESS_SERV)
		ret = create_server_conn(&sess->conn, port);

	if (!ret)
		sess->state = SESS_INITED;
	else
		return -1;

	return 0;
}

void deinit_session(struct session *sess)
{
	/* Release the connection */
	/* Fix ME */

	/* Cleanup the state */
	sess->state = SESS_UNINITED;
	return;
}

/* 
 * return the session pointer when succeed
 * return NULL when fails
 */
struct session *create_session(enum session_type type, char *addr, int port)
{
	int ret;
	struct session *new_sess = 
			(struct session*)malloc(sizeof(struct session));

	if (!new_sess)
		return NULL;

	memset(new_sess, 0, sizeof(struct session));
	ret = init_session(new_sess, type, addr, port);
	
	if (ret) {
		deinit_session(new_sess);
		new_sess = NULL;
	}

	return new_sess;
}

void release_session(struct session *sess)
{
	if (!sess)
		return;
	deinit_session(sess);
	free(sess);
	return; 
}
