#ifndef _PCI_H_
#define _PCI_H_

#include <stdbool.h>
#include "list.h"

struct pci_dev ;
struct pci_bus ;

struct pci_dev {
	struct list_head bus_list;
	struct pci_dev  *parent;
	struct pci_bus  *subordinate;
	char   name[20];
	struct resource resource[4];
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
#endif//_PCI_H_
