{
	if ($0 ~ /pio_write/) {
		ops["W:"$8]++
	} else if ($0 ~ /pio_read/) {
		ops["R:"$8]++
	}
}

END {
	for (op in ops) {
		printf "%-16.16s %d\n", op, ops[op]
	}
}
