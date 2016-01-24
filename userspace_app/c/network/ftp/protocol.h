/*
 * =====================================================================================
 *
 *       Filename:  protocol.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/29/2014 05:34:12 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include "net_connection.h"

enum protocol_type {
	RAW_TCP,
};

struct protocol {
	enum protocol_type type;
};

#endif //_PROTOCOL_H_
