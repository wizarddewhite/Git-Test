#!/bin/bash
ssh root@$1 -p 26 "/root/tasks/add_config $2 $3 && service v2ray restart"

