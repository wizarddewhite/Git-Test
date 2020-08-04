BEGIN {r1 = 0; r2 = 0}

# retrive result from first file
FNR==NR{
	r1 = $0
	next
}

# retrive result from second file
{
	r2 = $0
}

END { 
	printf "Difference: %10.2f%\n", (r1 - r2) * 100 / r1
}
