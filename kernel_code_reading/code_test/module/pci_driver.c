/*
 * =====================================================================================
 *
 *       Filename:  pci_driver.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/28/2013 03:15:29 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/pci.h>
MODULE_LICENSE("Dual BSD/GPL");

/* This is a test driver for a specific e1000e type
 *
 * Ethernet controller: Intel Corporation 82579LM Gigabit Network Connection 
 */
static DEFINE_PCI_DEVICE_TABLE(test_e1000_pci_tbl) = {
	{ PCI_VDEVICE(INTEL, 0x1502), 0 },
	{ 0, 0, 0, 0, 0, 0, 0 }	/* terminate list */
};
MODULE_DEVICE_TABLE(pci, test_e1000_pci_tbl);

char test_e1000_driver_name[] = "e1000e_test";

static int test_e1000_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	int err, bar;
	u16 vendor, device;

	/* access the config space */
	pci_read_config_word(pdev, PCI_VENDOR_ID, &vendor);
	pci_read_config_word(pdev, PCI_DEVICE_ID, &device);
	pr_info("The vendor_id is %x, the device_id is %x\n", vendor, device);

	err = pci_enable_device_mem(pdev);

	/* IO BARs */
	bar = pci_select_bars(pdev, IORESOURCE_IO);
	pr_info("Availale IO BARs are %x\n", bar);

	/* Request a IO region */
	err = pci_request_selected_regions_exclusive(pdev,
			bar, test_e1000_driver_name);

	/* MEM BARs */
	bar = pci_select_bars(pdev, IORESOURCE_MEM);
	pr_info("Availale MEM BARs are %x\n", bar);

	/* Request a MEM region */
	err = pci_request_selected_regions_exclusive(pdev,
			bar, test_e1000_driver_name);

	return err;
}

static void test_e1000_remove(struct pci_dev *pdev)
{
	pci_release_selected_regions(pdev, 
			pci_select_bars(pdev, IORESOURCE_IO|IORESOURCE_MEM));

	pci_disable_device(pdev);
}

static struct pci_driver test_e1000e_driver = {
	.name     = test_e1000_driver_name,
	.id_table = test_e1000_pci_tbl,
	.probe    = test_e1000_probe,
	.remove   = test_e1000_remove,
};

static int pci_driver_test_init(void)
{
	int ret;
	pr_info("A test driver for pci device\n");
	ret = pci_register_driver(&test_e1000e_driver);

        return ret;
}
static void pci_driver_test_exit(void)
{
	pci_unregister_driver(&test_e1000e_driver);
}
module_init(pci_driver_test_init);
module_exit(pci_driver_test_exit);
