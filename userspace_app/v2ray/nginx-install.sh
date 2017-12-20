#!/bin/bash

apt-get install nginx
cp nginx.conf /etc/nginx/nginx.conf
cp nginx-default /etc/nginx/sites-available/default
