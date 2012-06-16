/*
 * =====================================================================================
 *
 *       Filename:  kernel.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/16/2012 07:59:32 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef _LINUX_KERNEL_
#define _LINUX_KERNEL_


#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define container_of(ptr, type, member) ({                      \
	const typeof( ((type *)0)->member) *__mptr = (ptr);     \
	(type *)( (char*)__mptr -  offsetof(type, member) );})

#endif // _LINUX_KERNEL_
