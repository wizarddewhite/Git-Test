# transfile.awk
#
# Give the user a hook for filename transitions
#
# The user must supply functions beginfile() and endfile()
# that each take the name of the file being started or
# finished, respectively.




FILENAME != _oldfilename \
{
    if (_oldfilename != "")
        endfile(_oldfilename)
    _oldfilename = FILENAME
    beginfile(FILENAME)
}

END   { endfile(FILENAME) }

# test
#function beginfile(filename   )
#{
#	print "begin of file: "filename
#}
#
#function endfile(filename   )
#{
#	print "end of file: " filename
#}
#
#BEGIN {
#	print "test"
#}
