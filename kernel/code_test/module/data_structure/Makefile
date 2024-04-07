# If KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq ($(KERNELRELEASE),)
    obj-m += idr_index.o ida_idr_test.o
    obj-m += rb_tree.o rbtree_augmented.o
    obj-m += klist_test.o list_test.o
    obj-m += radix_tree.o xarray.o
    xarray-y := xarray_main.o xarray_debug.o
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
