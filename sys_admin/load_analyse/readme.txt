1. Top

shift + "p": order by cpu usage
shift + "m": order by memory usage

2. iostat

iostat -x 1 10

rrqm/s: 每秒进行 merge 的读操作数目。即 delta(rmerge)/s
wrqm/s: 每秒进行 merge 的写操作数目。即 delta(wmerge)/s
r/s:    每秒完成的读 I/O 设备次数。即 delta(rio)/s
w/s:    每秒完成的写 I/O 设备次数。即 delta(wio)/s
rsec/s: 每秒读扇区数。即 delta(rsect)/s
wsec/s: 每秒写扇区数。即 delta(wsect)/s
rkB/s:  每秒读K字节数。是 rsect/s 的一半，因为每扇区大小为512字节。(需要计算)
wkB/s:  每秒写K字节数。是 wsect/s 的一半。(需要计算)
avgrq-sz: 平均每次设备I/O操作的数据大小 (扇区)。delta(rsect+wsect)/delta(rio+wio)
avgqu-sz: 平均I/O队列长度。即 delta(aveq)/s/1000 (因为aveq的单位为毫秒)。
await:  平均每次设备I/O操作的等待时间 (毫秒)。即 delta(ruse+wuse)/delta(rio+wio)
svctm:  平均每次设备I/O操作的服务时间 (毫秒)。即 delta(use)/delta(rio+wio)
%util:   一秒中有百分之多少的时间用于 I/O 操作，或者说一秒中有多少时间 I/O 队列是非空的。即 delta(use)/s/1000 (因为use的单位为毫秒)

3. /proc/loadavg

/proc/loadavg
              The  first  three fields in this file are load average figures giving the number of jobs in the run queue (state
              R) or waiting for disk I/O (state D) averaged over 1, 5, and 15 minutes.  They are the same as the load  average
              numbers  given  by  uptime(1) and other programs.  The fourth field consists of two numbers separated by a slash
              (/).  The first of these is the number of currently executing kernel scheduling entities  (processes,  threads);
              this  will  be  less  than  or  equal  to the number of CPUs.  The value after the slash is the number of kernel
              scheduling entities that currently exist on the system.  The fifth field is the PID of the process that was most
              recently created on the system.

load average usually be less than 2(?).

4. Case Study

4.1 idle system with high load

Behavior: loadaverage in top is high, while id shows system is idle.

Reason: contain "D+" process. Use ps -axjf to list "D+" process.

4.2 kswapd is high

Behavior: top shows sy is high. And the one use cpu most is kswapd0.

Check: use free to verify the memory is low.

0. Reference:

https://blog.csdn.net/ahjxhy2010/article/details/100144077
https://www.cnblogs.com/wuchanming/p/9294678.html
https://help.aliyun.com/document_detail/41225.html
