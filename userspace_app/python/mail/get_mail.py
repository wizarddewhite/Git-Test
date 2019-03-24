#!/usr/bin/python
import mailbox

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
    iterate_mailbox()
