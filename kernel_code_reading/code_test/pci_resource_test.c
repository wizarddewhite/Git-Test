/*
 * =====================================================================================
 *
 *       Filename:  pci_resource_test.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/14/2013 11:29:10 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#include <string.h>

#include "resource.h"
#include "resource_case.h"
#include "resource_request_simulation.h"

int main()
{
	pci_init();
	pci_get_max_depth();
	return 0;
}
