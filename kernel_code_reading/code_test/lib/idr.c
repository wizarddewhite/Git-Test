/*
 * =====================================================================================
 *
 *       Filename:  idr.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/20/2012 10:33:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include <string.h>
#include "idr.h"

/**
 * idr_init - initialize idr handle
 * @idp:	idr handle
 *
 * This function is use to set up the handle (@idp) that you will pass
 * to the rest of the functions.
 */
void idr_init(struct idr *idp)
{
	memset(idp, 0, sizeof(struct idr));
	//spin_lock_init(&idp->lock);
}
