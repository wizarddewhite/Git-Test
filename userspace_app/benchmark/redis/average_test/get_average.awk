BEGIN {sum = 0}

{
	sum += $0
}

END { 
	# printf "Total:   %10.3f\n", sum
	printf "Average: %10.3f\n", sum / NR
}
