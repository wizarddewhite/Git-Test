#!/bin/bash

pushd tools/testing/memblock/
if [[ $? != 0 ]]; then
	echo Error: Not in kernel dir?
	exit
fi
echo === Run memblock test
make > /dev/null
./main
echo === memblock test finish
echo === Run memblock test with NUMA
make NUMA=1 > /dev/null
./main
echo === memblock test with NUMA finish
popd
