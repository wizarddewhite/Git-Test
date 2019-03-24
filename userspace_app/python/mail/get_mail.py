#!/usr/bin/python
import datetime
import mailbox
from dateutil.parser import parse

# a set of threads, each thread is a set of its messages
threads = []
# a dictionary of {message-id, thread}
message_ids = {}
# a dictionary of {in-reply-id, thread}
in_reply_ids = {}

def put_to_thread(message):
    if message['in-reply-to'] in message_ids:
        # is it reply to some message?
        thread = message_ids[message['in-reply-to']]
        # add the message to thread
        thread.add(message)
    elif message['in-reply-to'] and message['in-reply-to'] in in_reply_ids:
        # is it reply to the same message as others?
        thread = in_reply_ids[message['in-reply-to']]
        # add the message to thread
        thread.add(message)
    else:
        thread = {message}
        #add thread to threads
        threads.append(thread)

    # add message-id to dictionary
    in_reply_ids[message['in-reply-to']] = thread
    message_ids[message['message-id']] = thread

def threadify(mbox_name):
    mbox = mailbox.mbox(mbox_name)
    mbox.lock()
    #for message in mbox:
    for idx, message in mbox.iteritems():
        put_to_thread(message)
    mbox.flush()
    mbox.close()

    for t in threads:
        print "=====Thread: %d msgs", len(t)
        for msg in t:
            datetime = parse(msg['date'])
            print datetime.date(), msg['from'], msg['subject']

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
    threadify('example.mbox')
