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

void init_session(struct session *sess, char *name,
		struct sockaddr_in *addr, enum session_type type)
{
	sess->type = type;
	return;
}

void deinit_session(struct session *sess)
{
	sess->type = SESS_NONE;
}

struct session *create_session()
{
	struct session *new_sess = 
			(struct session*)malloc(sizeof(struct session));

	if (!new_sess)
		return NULL;

	memset(new_sess, 0, sizeof(struct session));
	init_session(new_sess, NULL, NULL, SESS_CLIENT);

	return new_sess;
}

int release_session(struct session *sess)
{
	if (!sess)
		return 0;
	deinit_session(sess);
	free(sess);
	return 0;
}
