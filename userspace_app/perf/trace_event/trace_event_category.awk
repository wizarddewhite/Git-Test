BEGIN {
	FS = ":"
}

/Tracepoint event/ {lib[$1]++}

END {
	for (l in lib) {
		printf "%-16.16s: %d\n", l, lib[l]
	}
}
