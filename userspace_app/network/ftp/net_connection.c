/*
 * =====================================================================================
 *
 *       Filename:  net_connection.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/25/2014 10:19:58 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "session.h"
#include "net_connection.h"

int raw_tcp_read(struct net_connection *conn, char *buff, int len)
{
	return recv(conn->sock, buff, len, 0);
}

int raw_tcp_write(struct net_connection *conn, char *buff, int len)
{
	return send(conn->sock, buff, len, 0);
}

struct net_ops raw_tcp_ops = {
	.read = raw_tcp_read,
	.write = raw_tcp_write,
};

int create_server_conn(struct net_connection *serv_conn, int port)
{
	int sock;

	if (serv_conn->state & CONN_CREATED)
		return -1;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		return -1;
	}
	serv_conn->state = CONN_CREATED;

	serv_conn->addr.sin_family = AF_INET;
	serv_conn->addr.sin_port = htons(port);
	serv_conn->addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(serv_conn->addr.sin_zero),8);

	if (bind(sock, (struct sockaddr *)&serv_conn->addr, sizeof(struct sockaddr)) == -1) {
		serv_conn->state &= ~CONN_CREATED;
		perror("Unable to bind");
		return -1;
	}

	if (listen(sock, 5) == -1) {
		serv_conn->state &= ~CONN_CREATED;
		perror("Listen");
		return -1;
	}
	serv_conn->state |= CONN_LISTEN;
	serv_conn->sock = sock;
	return 0;
}

int create_client_conn(struct net_connection *conn, char *addr, int port)
{
	int sock;

	if (conn->state & CONN_CREATED)
		return -1;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		return -1;
	}
	conn->state = CONN_CREATED;

	conn->addr.sin_family = AF_INET;
	conn->addr.sin_port = htons(port);
	inet_aton(addr, &conn->addr.sin_addr);
	bzero(&(conn->addr.sin_zero),8);

        if (connect(sock, (struct sockaddr *)&conn->addr,
                    sizeof(struct sockaddr)) == -1) {
		conn->state &= ~CONN_CREATED;
		perror("Connect");
		return -1;
        }
	conn->state |= CONN_CONNECT;
	conn->sock = sock;
	return 0;
}

int destroy_conn(struct net_connection *conn)
{
	if (!(conn->state & CONN_CREATED))
		return -1;

	close(conn->sock);
	return 0;
}

void set_net_ops(struct net_connection *conn, struct net_ops *ops)
{
	if (!conn)
		return;

	if (!ops)
		ops = &raw_tcp_ops;

	conn->ops = ops;

	return;
}

int net_conn_read(struct net_connection *conn, char *buff, int len)
{
	return conn->ops->read(conn, buff, len);
}

int net_conn_write(struct net_connection *conn, char *buff, int len)
{
	return conn->ops->write(conn, buff, len);
}
