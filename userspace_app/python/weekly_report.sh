#/bin/bash
# this is a bash script which is used to help send weekly report.
# This script use the monday.py to output the date range.
# The detailed report is in report.txt.
timestamp=`/home/ywywyang/weekly_report/monday.py`
mail -s "Weekly Report of Richard ($timestamp)" -r "Wizard <wizarddewhite@gmail.com>" address@mail.list.com < /home/report.txt
