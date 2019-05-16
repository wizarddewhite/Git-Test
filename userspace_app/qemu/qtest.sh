# reference
https://www.linux-kvm.org/images/4/43/03x09-TestingQEMU.pdf

# run all
make check-qtest
make check-help

# run one

## compile
make tests/bios-tables-test

## run
QTEST_QEMU_BINARY=x86_64-softmmu/qemu-system-x86_64 tests/bios-tables-test

# debug

## print operations between qemu
QTEST_LOG=1

## stop for gdb
QTEST_STOP=1  
