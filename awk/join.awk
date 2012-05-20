# join.awk --- join an array into a string

function join(array, start, end, sep,    result, i)
{
    if (sep == "")
       sep = " "
    else if (sep == SUBSEP) # magic value
       sep = ""
    result = array[start]
    for (i = start + 1; i <= end; i++)
        result = result sep array[i]
    return result
}

#BEGIN {
#	arr[1] = "T"
#	arr[2] = "h"
#	arr[3] = "i"
#	arr[4] = "s"
#	arr[5] = " "
#	arr[6] = "i"
#	arr[7] = "s"
#	arr[8] = " "
#	arr[9] = "a"
#	arr[10] = " "
#	arr[11] = "g"
#	arr[12] = "o"
#	arr[13] = "o"
#	arr[14] = "d"
#	arr[15] = " "
#	arr[16] = "d"
#	arr[17] = "a"
#	arr[18] = "y"
#	arr[19] = "!"
#
#	print join(arr, 1, 19, SUBSEP)
#}
