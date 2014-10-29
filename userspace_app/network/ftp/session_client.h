/*
 * =====================================================================================
 *
 *       Filename:  session_client.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/25/2014 06:13:08 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _SESS_CLIENT_
#define _SESS_CLIENT_

#include <sys/socket.h>
#include <sys/types.h>

#include "session.h"
#include "net_connection.h"

int init_client_sess(char *serv_name, int port, struct net_ops *ops);
int deinit_client_sess();
struct session *get_client_sess();

#endif // _SESS_CLIENT_
