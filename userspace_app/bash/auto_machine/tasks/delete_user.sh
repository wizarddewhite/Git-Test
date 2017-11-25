#!/bin/bash
ssh root@$1 -p 26 "userdel $2 && rm -rf /home/$2"

