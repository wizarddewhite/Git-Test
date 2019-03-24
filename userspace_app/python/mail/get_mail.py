#!/usr/bin/python
from datetime import datetime, timedelta
import mailbox
from dateutil.parser import parse

# a set of threads, each thread is a set of its messages
threads = []
# a dictionary of {message-id, thread}
message_ids = {}
# a dictionary of {in-reply-id, thread}
in_reply_ids = {}
# a dictionary of {references, thread}
references = {}

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
        thread.add(message)
    elif message['in-reply-to'] and message['in-reply-to'] in in_reply_ids:
        # is it reply to the same message as others?
        thread = in_reply_ids[message['in-reply-to']]
        # add the message to thread
        thread.add(message)
    else:
        # last try on references
        thread = search_reference(message)
        if thread:
            thread.add(message)
        else:
            new_thread = True
            thread = {message}
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
        date = parse(message['date']).replace(tzinfo=None)
        if date < start or date > end:
            continue
        put_to_thread(message)
    mbox.flush()
    mbox.close()

    print "Total %d threads" % len(threads)
    for t in threads:
        print("=====Thread: %d msgs" % (len(t)))
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
    #start = datetime.now() - timedelta(weeks=2)
    #end = datetime.now()
    start = parse("2018-08-15")
    end = parse("2018-08-18")
    threadify('example.mbox', start, end)
