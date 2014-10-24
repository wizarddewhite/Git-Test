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

enum session_type {
	SESS_USER,
	SESS_SERV
};

struct session {
	enum session_type type;
	char remote_host[64];
	struct sockaddr_in remote_addr;
	struct sockaddr_in local_addr;
};

void init_session(struct session *sess, char *name,
		struct sockaddr_in *addr, enum session_type);
struct session *create_session();

#endif //_SESSION_H_
