#!/bin/bash
ssh root@$1 -p 26 "useradd -s /bin/true -d /home/$2 -m $2 && mkdir -p /home/$2/.ssh"

