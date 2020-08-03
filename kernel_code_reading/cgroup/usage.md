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

1. Create a new controller under cpuset

mkdir /sys/fs/cgroup/cpuset/tiger

2. Limit resources for this controller

echo "0-1" > /sys/fs/cgroup/cpuset/tiger/cpuset.cpus
echo 0 > /sys/fs/cgroup/cpuset/tiger/cpuset.mems

3. Validate

# echo current shell process number to cgroup.procs
# so all child will inherit this attribute
echo $$ > /sys/fs/cgroup/cpuset/tiger/cgroup.procs

# stress on 4 cpus
stress -c 4

# while only 2 cpu is working
top
