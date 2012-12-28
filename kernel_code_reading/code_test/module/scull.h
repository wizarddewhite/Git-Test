/*
 * =====================================================================================
 *
 *       Filename:  scull.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/16/2012 09:21:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _SCULL_H_
#define _SCULL_H_

#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>		/* kmalloc() */

#define SCULL_SIZE    1000

struct scull_dev {
	char * data;              /* Pointer to the date this device has */
	unsigned long size;       /* amount of data stored here */
	struct semaphore sem;     /* mutual exclusion semaphore */
	struct cdev cdev;	  /* Char device structure      */
};

#endif // _SCULL_H_
