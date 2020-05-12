#!/usr/bin/python
from datetime import datetime, timedelta
import mailbox
from dateutil.parser import parse
import re
import argparse
import calendar

def filter_mail(mbox_name, keyword):

    print "Filter keyword: ", keyword
    print "Related mail would be saved to ", mbox_name + "_" + keyword.lower()

    mbox = mailbox.mbox(mbox_name)
    mbox_new = mailbox.mbox(mbox_name + "_" + keyword.lower())
    mbox.lock()

    for idx, message in mbox.iteritems():
        if not message['subject']:
            continue
        if re.search(keyword, message['subject'], re.IGNORECASE):
            # print message['subject']
            mbox_new.add(message)

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
    parser.add_argument('--keyword', metavar='filter', required=True,
                        help='Keyword to filter mail')
    args = parser.parse_args()

    filter_mail(args.mailbox, args.keyword)
