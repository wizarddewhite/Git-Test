#!/bin/bash
# */5 * * * *    root    /root/Mail/fetchmail_cron.sh
printf "refresh oauth2 " >> /root/Mail/fetchmail.log
date >> /root/Mail/fetchmail.log

# /root/Mail/fetchmail-oauth2.py -c /root/Mail/oauth.conf --obtain_refresh_token_file

/root/Mail/fetchmail-oauth2.py -c /root/Mail/oauth.conf --auto_refresh
/usr/local/bin/fetchmail -q
/usr/local/bin/fetchmail

