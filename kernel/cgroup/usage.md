Here is an example of how to use cpuset to limit cpu and memory of process,
which refers to https://www.cnblogs.com/shishaochen/p/9735114.html

0. Mount cgroup fs

Usually they are mounted under /sys/fs/cgroup.

If not, run the following script

```
cgroot="${1:-/sys/fs/cgroup}"
subsys="${2:-blkio cpu cpuacct cpuset devices freezer memory net_cls net_prio ns perf_event}"
mount -t tmpfs cgroup_root "${cgroot}"
for ss in $subsys; do
  mkdir -p "$cgroot/$ss"
  mount -t cgroup -o "$ss" "$ss" "$cgroot/$ss"
done
```

0.1 CoMount two controller

According to the man page:
https://man7.org/linux/man-pages/man7/cgroups.7.html

It is possible to comount multiple controllers against the same hierarchy. 

$ mount -t cgroup -o cpu,cpuacct none /sys/fs/cgroup/cpu,cpuacct

1. Limit cpu resource for process

1.1 Create a new controller under cpuset

mkdir /sys/fs/cgroup/cpuset/tiger

1.2. Limit cpu resources for this controller

echo "0-1" > /sys/fs/cgroup/cpuset/tiger/cpuset.cpus
echo 0 > /sys/fs/cgroup/cpuset/tiger/cpuset.mems

1.3. Validate

# echo current shell process number to cgroup.procs
# so all child will inherit this attribute
echo $$ > /sys/fs/cgroup/cpuset/tiger/cgroup.procs

# stress on 4 cpus

$ stress -c 4

# while only 2 cpu is working

$ top

2. Limit memory resource for process

2.1 Create a new controller under memory

mkdir /sys/fs/cgroup/memory/rabbit

2.2. Limit memory resources for this controller

# limit 4M
echo 4M > /sys/fs/cgroup/memory/rabbit/memory.limit_in_bytes

2.3. Validate

# echo current shell process number to cgroup.procs
# so all child will inherit this attribute
echo $$ > /sys/fs/cgroup/cpuset/rabbit/cgroup.procs

# disable swap
echo 0 > /sys/fs/cgroup/cpuset/rabbit/memory.swappiness

# eat memory

```
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MB (1024 * 1024)

int main(int argc, char *argv[])
{
    char *p;
    int i = 0;
    while(1) {
        p = (char *)malloc(MB);
        memset(p, 0, MB);
        printf("%dM memory allocated\n", ++i);
        sleep(1);
    }

    return 0;
}
```

Then running this program will receive oom.

# cat memory.oom_control
oom_kill_disable 0
under_oom 0
oom_kill 1
