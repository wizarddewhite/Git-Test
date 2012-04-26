#Print the length of the longest input line
awk '{ if (length($0) > max) max = length($0) } END {print max} data
#Print every line that is longer than 80 characters
awk 'length($0) > 80' data
#Print every line that has at least one field:
awk 'NF > 0' data
#print seven random numbers from 0 to 100
awk 'BEGIN { for (i = 1; i <= 7; i++) print int(101 * rand()) }'
#print the total number of bytes used by files
ls -l files | awk '{ x += $5 } END { print "total bytes: " x }'
#Print a sorted list of the login names of all users
awk -F: '{ print $1 }' /etc/passwd | sort
#Print the even-numbered lines in the data file:
awk 'NR % 2 == 0' data

#====================================================
# Regex
#====================================================
# match string for whole line
awk '/foo/ {print }' BBS-list
# match string for one field
awk '$1 ~ /J/' inventory-shipped
# different effect of the two commands below
awk ' BEGIN {OFS = ":"} { print $0 }' inventory-shipped
awk ' BEGIN {OFS = ":"} {$1=$1; print $0 }' inventory-shipped
awk '{if ($1 ~ /J/) print}' inventory-shipped
# don't match string one filed
awk '$1 !~ /J/' inventory-shipped

#====================================================
# Reading Input File
#====================================================
# change the recode seperator to '/'
awk 'BEGIN {RS = "/"}; {print $0}' BBS-list
# NF is the number of fields in the current record
awk '/foo/ { print $1, NF }' BBS-list
