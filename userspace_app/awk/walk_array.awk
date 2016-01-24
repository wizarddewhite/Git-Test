function walk_array(arr, name,      i)
{
	for (i in arr) {
		if (isarray(arr[i]))
			walk_array(arr[i], (name "[" i "]"))
		else
			printf("%s[%s] = %s\n", name, i, arr[i])
	}
}

#test code
#BEGIN {
#	a[1] = 1
#	a[2][1] = 21
#	a[2][2] = 22
#	a[3] = 3
#	a[4][1][1] = 411
#	a[4][2] = 42
#
#	walk_array(a, "a")
#}
