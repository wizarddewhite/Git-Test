#!/bin/bash
ssh root@$1 -p 26 "usermod -g $2 -G $2 $2"

