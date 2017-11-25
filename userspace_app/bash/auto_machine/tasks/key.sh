#!/bin/bash
scp -P 26 /home/$2/.ssh/authorized_keys root@$1:/home/$2/.ssh/

