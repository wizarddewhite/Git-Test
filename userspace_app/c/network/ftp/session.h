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
	SESS_CLIENT,
	SESS_SERV
};

enum session_state {
	SESS_INITED,
	SESS_UNINITED,
};

struct session {
	enum session_type type;
	enum session_state state;
	struct net_connection conn;
};

int init_session(struct session *sess, enum session_type type,
		char *addr, int port);
void deinit_session(struct session *sess);
struct session *create_session();
void release_session(struct session *sess);

#endif //_SESSION_H_
