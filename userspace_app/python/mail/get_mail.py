#!/usr/bin/python
import mailbox

if __name__ == "__main__":
    mbox = mailbox.mbox('example.mbox')
    mbox.lock()
    for message in mbox:
        if 'Re' in message['subject']:
            continue
        print(message['subject'])
    mbox.flush()
    mbox.close()
