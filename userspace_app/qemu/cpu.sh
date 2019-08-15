# show available cpu model and cpuid
qemu-system-x86_64 -cpu help

# specify cpu type

## use host cpu type
-cpu host

# specify cpuid with + or -
-cpu host,+movdir64b,-movdiri

# specify phys-bits
-cpu xxx,phys-bits=46
