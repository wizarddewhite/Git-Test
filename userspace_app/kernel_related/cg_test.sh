#!/bin/bash
mkdir /sys/fs/cgroup/memory/test
# echo 100M > /sys/fs/cgroup/memory/test/memory.limit_in_bytes
echo 100M > /sys/fs/cgroup/memory/test/memory.soft_limit_in_bytes
echo $$ > /sys/fs/cgroup/memory/test/cgroup.procs
echo 1800000 > /proc/sys/vm/min_free_kbytes

for ((i=0;i<2;i++));
do
        memhog 1500M
done

# echo $$ > /sys/fs/cgroup/memory/cgroup.procs
# rmdir test
