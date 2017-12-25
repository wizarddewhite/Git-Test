#!/bin/bash
ssh root@$1 -p 26 "/root/tasks/del_config $2 && service v2ray restart"

