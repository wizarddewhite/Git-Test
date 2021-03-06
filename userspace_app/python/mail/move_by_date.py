#!/usr/bin/python
from datetime import datetime, timedelta
import mailbox
from dateutil.parser import parse
import re
import argparse
import calendar

def move_mail(mbox_name, month):

    _, ld = calendar.monthrange(month.year, month.month)
    start = month.replace(day=1)
    end =  month.replace(day=ld)
    end += timedelta(days=1)
    print "start: ", start
    print "end  : ", end

    mbox = mailbox.mbox(mbox_name)
    mbox_new = mailbox.mbox(mbox_name + "_" + month.strftime('%Y%m'))
    mbox.lock()

    for idx, message in mbox.iteritems():
        if not message['date']:
            continue
        date = parse(re.sub('\(.*?\)', '', message['date'])).replace(tzinfo=None)
        if date < start or date > end:
            continue
        #print message['subject']
        mbox_new.add(message)
        mbox.remove(idx)

    mbox.flush()
    mbox.close()
    mbox_new.flush()
    mbox_new.close()

    return

if __name__ == "__main__":
    #iterate_mailbox()

    parser = argparse.ArgumentParser(description='Processing mailbox')
    parser.add_argument('--mailbox', metavar='path', required=True,
                        help='the file of mailbox copy mail from')
    parser.add_argument('--month', metavar='date', required=True,
                        help='the date of mail to extract in month granularity')
    args = parser.parse_args()

    month = parse(args.month)

    move_mail(args.mailbox, month)
