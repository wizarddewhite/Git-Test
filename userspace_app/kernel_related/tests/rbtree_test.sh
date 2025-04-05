#!/bin/bash

pushd tools/testing/rbtree/
if [[ $? != 0 ]]; then
	echo Error: Not in kernel dir?
	exit
fi

echo === Run rbtree test
make rbtree_test > /dev/null
./rbtree_test
echo === rbtree test finish

echo === Run interval tree test
make interval_tree_test > /dev/null
./interval_tree_test
echo === interval tree test finish
popd
