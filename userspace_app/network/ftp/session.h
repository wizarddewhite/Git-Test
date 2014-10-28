/*
 * =====================================================================================
 *
 *       Filename:  session.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/24/2014 11:14:39 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _SESSION_H_
#define _SESSION_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "net_connection.h"

enum session_type {
	SESS_NONE,
	SESS_CLIENT,
	SESS_SERV
};

enum session_ret {
	RET_SESS_INIED,
	RET_SESS_UNINIED,
	RET_SESS_SUCCESS
};

struct session {
	enum session_type type;
	struct net_connection conn;
};

void init_session(struct session *sess, char *name,
		struct sockaddr_in *addr, enum session_type);
void deinit_session(struct session *sess);
struct session *create_session();
int release_session(struct session *sess);

#endif //_SESSION_H_
