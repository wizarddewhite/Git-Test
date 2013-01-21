#!/usr/bin/python
# this script is run on Friday to calculate the date of Monday.
# And output the date range of this week
import string
import platform
import types
import time
from datetime import date, timedelta

today = date.today()
monday = today - timedelta(4)
print "%d/%d-%d/%d" %(monday.month, monday.day, today.month, today.day)

