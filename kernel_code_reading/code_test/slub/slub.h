/*
 * =====================================================================================
 *
 *       Filename:  slub.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/29/2015 09:30:43
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _SLUB_H_
#define _SLUB_H_

#define max(a,b) ( ((a)>(b)) ? (a):(b) )
#define min(a,b) ( ((a)>(b)) ? (b):(a) )

int calculate_order(int size, int reserved);

#endif
