# qemu commandline
-device vfio-pci,host=0000:00:14.0,id=hostdev0,bus=pci.0

# kernel module preparation
intel_iommu=on

sudo modprobe vfio
sudo modprobe vfio-pci

# attach pci_dev to vfio-pci
## show devices in the same iommu group
$ ls /sys/bus/pci/devices/0000:00:14.0/iommu_group/devices
0000:00:14.0  0000:00:14.2

# remove driver for these devices

# show device id 
$ lspci -ns 0000:00:14.0
00:14.0 0c03: 8086:a36d (rev 10)

$ lspci -ns 0000:00:14.2
00:14.2 0500: 8086:a36f (rev 10)

# attach them to vfio-pci
echo 8086 a36d > /sys/bus/pci/drivers/vfio-pci/new_id
echo 8086 a36f > /sys/bus/pci/drivers/vfio-pci/new_id
