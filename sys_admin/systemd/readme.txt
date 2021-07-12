1. Analysis services

1.1 List time consumed for each service

$ systemd-analyze blame

1.2 Plot service start sequence

$ systemd-analyze plot > boot.svg

1.3 List Dependency

$ systemctl list-dependencies --after timers.target

$ systemctl list-dependencies --before timers.target

2. How to add an service

https://www.tecmint.com/create-new-service-units-in-systemd/

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
