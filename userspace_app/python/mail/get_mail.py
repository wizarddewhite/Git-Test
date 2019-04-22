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
#   [4] files
subjects = []
# a dictionary of {title, subject}
titles = {}

# a dictionary of {participants, #messages}
participants = {}
# a dictionary of {companies, #messages}
companies = {}

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

def put_to_participants(mail_from):
    #mail_from = mail_from.lower()
    if mail_from in participants:
        #print "old parti", mail_from, participants[mail_from]
        participants[mail_from] = participants[mail_from] + 1
    else:
        participants[mail_from] = 1

def email_to_company(email):
    raw_email = re.search('<.*?>', email)
    if raw_email:
        email = email[raw_email.start() + 1:raw_email.end() - 1]
    return email.split('@')[1].split('.')[-2]

def put_to_companies(mail_from):
    c = email_to_company(mail_from)
    if c in companies:
        companies[c] = companies[c] + 1
    else:
        companies[c] = 1

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
        if re.search('patch', message['subject'], re.IGNORECASE):
            put_to_thread(message)
            put_to_participants(message['from'])
            put_to_companies(message['from'])
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

def get_files(thread):
    files = set()
    for message in thread:
        if re.search('re:', message['subject'], re.IGNORECASE):
            continue

        if message.is_multipart():
            content = ''.join(part.get_payload(decode=True) for part in message.get_payload())
        else:
            content = message.get_payload(decode=True)
        content_in_line = content.splitlines()

        for i, l in enumerate(content_in_line):
            if re.search('file changed', l):
                try:
                    num = int(l.split()[0])
                except ValueError:
                    continue;
                while num:
                    files.add(content_in_line[i-num].split()[0])
                    num -= 1
                break

    return files


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
            subject = [title, 1, replies, t[0]['from'], get_files(t)]
            titles[title] = subject
            subjects.append(subject)

    return
    for s in subjects:
        print s

def sort_subjects_by_replies(subject):
    return subject[2]

def show_subjects():
    total_messages = float(0)

    for _, t in participants.iteritems():
        total_messages += t

    if total_messages == 0:
        print "No subject extracted"
        return

    print "Total %d discussions in %d subjects" % (total_messages, len(subjects))
    print "Top 10 individual participants:"
    print "==============================="
    for p, t in sorted(participants.iteritems(),
                       key=lambda (p,t): (t,p),
                       reverse=True)[:10]:
        print "    %4d(%5.02f%%) %s" % (t, t * 100/total_messages, p)
    print "Top 10 company participants:"
    print "============================"
    for c, t in sorted(companies.iteritems(),
                       key=lambda (c,t): (t,c),
                       reverse=True)[:10]:
        print "    %5d(%5.02f%%) %-10s" % (t, t * 100/total_messages, c)

    for s in subjects:
        print "{:14}: {}".format('Title', s[0])
        print "    {:10}: {}".format('From', s[3])
        print "    {:10}: {}".format('Replies', s[2])
        print "    {:10}: {}".format('Rounds', s[1])
        print "    {:10}: {}".format('Files',  s[4])

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
    parser.add_argument('--start', metavar='date', required=False,
                        help='start date')
    parser.add_argument('--end', metavar='date', required=False,
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
