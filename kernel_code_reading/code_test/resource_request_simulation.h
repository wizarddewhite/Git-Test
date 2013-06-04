/*
 * =====================================================================================
 *
 *       Filename:  resource_request_simulation.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/30/2013 09:33:20 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef _RESOURCE_REQUEST_H_
#define _RESOURCE_REQUEST_H_

#include <stdbool.h>
#include "list.h"

struct pci_dev ;
struct pci_bus ;

struct pci_dev {
	struct pci_dev  *parent;
	struct pci_bus  *subordinate;
	char   name[20];
	struct resource resource[4];
	struct list_head bus_list;
};

struct pci_bus {
	struct list_head node;
	struct list_head children;
	struct list_head devices;
	struct pci_bus   *parent;
	int    number;
	char   name[20];
};

struct pci_depth {
	struct list_head node;
	int dep;
	struct pci_bus *bus;
};

void pci_init();

int pci_get_max_depth();

#endif//_RESOURCE_REQUEST_H_
