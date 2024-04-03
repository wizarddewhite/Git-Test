#!/bin/bash

pushd tools/testing/memblock/
if [[ $? != 0 ]]; then
	echo Error: Not in kernel dir?
	exit
fi
make
echo === Run memblock test
./main
echo === memblock test finish
popd
