# If KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq ($(KERNELRELEASE),)
    obj-m := hello_world.o oops_example.o
    obj-m += proc_fs.o seq_fs.o kobject-example.o
    obj-m += scull.o char_dev.o
    scull-objs := scull_sysfs.o
    obj-m += usermodehelper.o timer.o workqueue_test.o
    obj-m += pci_driver.o register_netdev.o my_debugfs.o
    obj-m += idr_index.o percpu_test.o
# hello_world-objs :=
# Otherwise we were called directly from the command
# line; invoke the kernel build system.
else
    KERNELDIR ?= /lib/modules/$(shell uname -r)/build
    PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
endif
