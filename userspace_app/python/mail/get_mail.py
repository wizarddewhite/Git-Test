#!/usr/bin/python
from datetime import datetime, timedelta
import mailbox
from dateutil.parser import parse
import re
import argparse

# a dictionary of {message-id, thread}
message_ids = {}
# a dictionary of {in-reply-id, thread}
in_reply_ids = {}
# a dictionary of {references, thread}
references = {}

# a set of threads, each thread is a list of its messages
threads = []

# a list of subjects
# subject:
#   [0] title
#   [1] rounds
#   [2] replies
#   [3] from
subjects = []
# a dictionary of {title, subject}
titles = {}

def dup_message_id(message):
    if message['message-id'] in message_ids:
        return True
    else:
        return False

def search_reference(message):
    if len(references) == 0:
        return None
    if message['references'] == None:
        return None
    for ref in message['references'].split():
        if ref in references:
            return references[ref]
    return None

def put_to_thread(message):
    new_thread = False
    if message['in-reply-to'] in message_ids:
        # is it reply to some message?
        thread = message_ids[message['in-reply-to']]
        # add the message to thread
        thread.append(message)
    elif message['in-reply-to'] and message['in-reply-to'] in in_reply_ids:
        # is it reply to the same message as others?
        thread = in_reply_ids[message['in-reply-to']]
        # add the message to thread
        thread.append(message)
    else:
        # last try on references
        thread = search_reference(message)
        if thread:
            thread.append(message)
        else:
            new_thread = True
            thread = [message]
            #add thread to threads
            threads.append(thread)

    # add message-id to dictionary
    in_reply_ids[message['in-reply-to']] = thread
    message_ids[message['message-id']] = thread

    # if this is a new thread and has references,
    # record this in case some broken thread
    if new_thread and message['references']:
        for ref in message['references'].split():
            references[ref] = thread

def threadify(mbox_name, start, end):
    mbox = mailbox.mbox(mbox_name)
    mbox.lock()
    #for message in mbox:
    for idx, message in mbox.iteritems():
        if dup_message_id(message):
            continue
        date = parse(re.sub('\(.*?\)', '', message['date'])).replace(tzinfo=None)
        if date < start or date > end:
            continue
        put_to_thread(message)
    mbox.flush()
    mbox.close()

    return

    print "Total %d threads" % len(threads)
    for t in threads:
        print("=====Thread: %d msgs" % (len(t)))
        for msg in t:
            datetime = parse(msg['date'])
            print datetime.date(), msg['from'], msg['subject']

def is_patch(thread):
    raw_title = thread[0]['subject']
    return re.search('patch', raw_title, re.IGNORECASE)

def get_title(thread):
    raw_title = thread[0]['subject']

    # strip 'Re'
    raw_title = re.sub('re:', '', raw_title, flags=re.IGNORECASE)
    # strip 'Patch'
    raw_title = re.sub('\[.*?\]', '', raw_title)
    # strip 'new line'
    raw_title = re.sub("\n|\r", "", raw_title)
    return raw_title.lstrip()

def get_replies(thread):
    replies = 0
    for msg in thread:
        raw_title = msg['subject']

        # strip 'Re'
        if re.search('re:', raw_title, re.IGNORECASE):
            replies += 1
    return replies

def get_subjects(only_patch):
    for t in threads:
        # only get patch thread
        if only_patch and not is_patch(t):
            continue

        title = get_title(t)

        replies = get_replies(t)
        if title in titles:
            subject = titles[title]
            subject[1] += 1
            subject[2] += replies
        else:
            subject = [title, 1, replies, t[0]['from']]
            titles[title] = subject
            subjects.append(subject)

    return
    for s in subjects:
        print s

def sort_subjects_by_replies(subject):
    return subject[2]

def show_subjects():
    print "Total %d subjects" % len(subjects)
    for s in subjects:
        print "{:14}: {}".format('Title', s[0])
        print "    {:10}: {}".format('From', s[3])
        print "    {:10}: {}".format('Replies', s[2])
        print "    {:10}: {}".format('Rounds', s[1])

def iterate_mailbox():
    mbox = mailbox.mbox('example.mbox')
    mbox.lock()
    for message_id, message in mbox.iteritems():
        if 'Re' in message['subject']:
            continue
        print("{:4}: {}".format(message_id, message['subject']))
    mbox.flush()
    mbox.close()

if __name__ == "__main__":
    #iterate_mailbox()

    parser = argparse.ArgumentParser(description='Processing mailbox')
    parser.add_argument('--mailbox', metavar='path', required=True,
                        help='the file of mailbox')
    parser.add_argument('--start', metavar='path', required=False,
                        help='start date')
    parser.add_argument('--end', metavar='path', required=False,
                        help='end date')
    args = parser.parse_args()

    start = datetime.now() - timedelta(weeks=2)
    if args.start:
        start = parse(args.start)
    end = datetime.now()
    if args.end:
        end = parse(args.end)

    #start = parse("2018-08-15")
    threadify(args.mailbox, start, end)
    get_subjects(True)

    subjects.sort(key=sort_subjects_by_replies, reverse=True)
    show_subjects()
