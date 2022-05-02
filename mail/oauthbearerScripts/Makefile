# Example /etc/postfix/Makefile
# Public Domain
#
# This probably needs to be configured properly.

# CONFIGURE "all" to build all needed postfix .db "map" files:
all: saslpass-autogen.db tls_policy.db relayhost_map.db
#all: saslpass-autogen.db saslpass.db virtual.db tls_policy.db relayhost_map.db

# CONFIGURE USERS:
#
# saslpass-autogen.db is generated from a combination of saslpass,
# XOAUTH2_MAP, and the filenames refered to by XOAUTH2.
#
# XOAUTH2_MAP contains one or more space-separated account records.
# Each record should contain 3 fields separated by a tilde (~):
#   1. The "key" to be used when generated saslpass-autogen.  Typically
#      a HOST:PORT, IP_ADDRESS:PORT, or if
#      smtp_sender_dependent_authentication=yes, it could be the name of
#      the email account (often USER@HOST).
#   2. The username to use when authenticating the SMTP connection.
#      (often USER@gmail.com or some other USER@HOST, but not
#      necessarily).  Sometimes fields 1 and 2 are identical.
#   3. The full path to a file that contains the access token to
#      use with the XOAUTH2 (or someday OAUTHBEARER, if the plugin
#      is enhanced and main.cf reconfigured).
#
# Then the saslpass-autogen rule below adjusts this data into an
# input file suitable to feeding into postmap to finally generate
# the saslpass-autogen.db file.
#
#XOAUTH2_MAP = \
#  USER_AT_HOST~ACCOUNT_NAME~/PATH/TO/ACCESS_TOKEN_PASSWORD \
#  HOST_COLON_PORT~ACCOUNT_NAME~/PATH/TO/ACCESS_TOKEN_PASSWORD

XOAUTH2_MAP = \
   CONFIG_KEY~CONFIG_USER~CONFIG_TOKEN

####
# Generic rules:

%.db: %
	postmap hash:$<

XOAUTH2_FILES:=$(foreach ENTRY,${XOAUTH2_MAP},$(word 3,$(subst ~, ,${ENTRY})))

# This is careful not to ever include the content of the password
# file on a command line "ps" listing, by splitting printf from head -1
# for a single output line:

saslpass-autogen: saslpass ${XOAUTH2_FILES} Makefile
	umask 077 ; \
	( cat saslpass ; \
	  sCmd='s/\([^~]*\)~\([^~]*\)~.*/\1 \2:/' ; \
	  for entry in ${XOAUTH2_MAP} ; do \
	    name="$$(echo "$$entry" | cut -d '~' -f 3)" ; \
	    [ -r "$$name" ] || { echo "Can't read $$name" 1>&2 ; exit 1 ; } ; \
	    printf "%s" "$$(echo "$$entry" | sed -e "$$sCmd")" ; \
	    head -1 "$$name" || exit 1 ; \
	  done ; \
	) > $@.tmp
	mv $@.tmp $@

env:
	env
	echo "also XOAUTH2_FILES: ${XOAUTH2_FILES}"
