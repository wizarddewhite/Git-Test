/*
 * =====================================================================================
 *
 *       Filename:  net_connection.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/25/2014 11:14:23 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _NET_CONN_H_
#define _NET_CONN_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

enum conn_state {
	CONN_UNUSE   = 0x00,
	CONN_CREATED = 0x01,
	CONN_LISTEN  = 0x02,
	CONN_CONNECT = 0x04,
};

struct net_connection {
	enum conn_state state;
	int sock;
	int port;
	struct sockaddr_in addr;
};

int create_server_conn(struct net_connection *serv_conn, int port);
int create_client_conn(struct net_connection *conn, char *addr, int port);
#endif // _NET_CONN_H_
