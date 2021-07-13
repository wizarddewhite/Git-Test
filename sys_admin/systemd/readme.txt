1. Analysis services

1.1 List time consumed for each service

$ systemd-analyze blame

1.2 Plot service start sequence

$ systemd-analyze plot > boot.svg

1.3 List Dependency

$ systemctl list-dependencies --after timers.target

$ systemctl list-dependencies --before timers.target

2. How to add an service

The following example is copied from:
https://www.tecmint.com/create-new-service-units-in-systemd/

And for a more interesting reference:
https://www.ruanyifeng.com/blog/2016/03/systemd-tutorial-commands.html
https://www.ruanyifeng.com/blog/2016/03/systemd-tutorial-part-two.html

For detailed primitives

https://www.freedesktop.org/software/systemd/man/systemd.unit.html

2.1 Create service file
$ cat /etc/systemd/system/connect.service
[Unit]
Description = making network connection up
After = network.target

[Service]
ExecStart = /root/scripts/conup.sh

[Install]
WantedBy = multi-user.target

2.2 Create executable file
$ cat /root/scripts/conup.sh
#!/bin/bash
touch /tmp/testbootfile

$ chmod +x /root/scripts/conup.sh

2.3 Enable service
$ systemctl enable connect.service

2.4 Start service
$ systemctl start connect.service

3. Target

Target is a replacement for runlevel.

3.1 List all targets

$ systemctl list-unit-files --type=target

3.2 List unit in one target

$ systemctl list-dependencies multi-user.target

3.3 Runlevel to Target

Traditional runlevel      New target name     Symbolically linked to...

Runlevel 0           |    runlevel0.target -> poweroff.target
Runlevel 1           |    runlevel1.target -> rescue.target
Runlevel 2           |    runlevel2.target -> multi-user.target
Runlevel 3           |    runlevel3.target -> multi-user.target
Runlevel 4           |    runlevel4.target -> multi-user.target
Runlevel 5           |    runlevel5.target -> graphical.target
Runlevel 6           |    runlevel6.target -> reboot.target

4. View Log

journalctl could view all those logs.

The configuration file is /etc/systemd/journald.conf.

4.1 Display log for particular service

$ sudo journalctl -u nginx.service
$ sudo journalctl -u nginx.service --since today
