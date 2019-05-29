BEGIN {
	first_dirty_pages=0;
	first_cycles=0;

	for (x = 0; x <10; x++) {
		dirty_pages[x]=0;
		cycles[x]=0;
		ve[x]=0;
		te[x]=0;
	}
	round=-1;

	while ((getline line < "/proc/cpuinfo") > 0) {
		if (line ~ /MHz/) {
			#print line
			split(line, resArr, " ")
			cpu_mhz=resArr[4]
			break
		}
	}
}

NR <=3 && /dirty_pages/ { 
	first_dirty_pages += $8
}
NR <=3 && /time used/ {
	first_cycles += $9
}
/kvm_test/ {
	round++
}
/dirty_pages/ { 
	dirty_pages[round] += $9
}
/time used/ {
	cycles[round] += $10
}
/Valid Entries/ {
	ve[round] = $10 + $11 + $12 + $13
}
/Touch Entries/ {
	te[round] = $10 + $11 + $12 + $13
}

END {
	printf "Result of %s:\n", FILENAME
	print "=============="
	#printf "Total Round: %d\n", round + 1
	#printf "First   Dirty Pages: %15.3f\n", first_dirty_pages
	#printf "First   Walk Cycles: %15.3f\n", first_cycles
	#printf "Average Dirty Pages: %15.3f\n", dirty_pages / round
	#printf "Average Walk Cycles: %15.3f\n", cycles / round
	printf "Total D Pages:"
	for (x = 0; x <10; x++) {
		if (dirty_pages[x] > 1024)
			printf "%9dM", (dirty_pages[x] * 4) / 1024
		else
			printf "%9dK", dirty_pages[x] * 4
	}
	printf "\n"
	printf "Total Cycles :"
	for (x = 0; x <10; x++) {
		printf "%10d", cycles[x]
	}
	printf "\n"
	printf "Time in ms   :"
	for (x = 0; x <10; x++) {
		printf "%10d", cycles[x] / cpu_mhz / 1000
	}
	printf "\n"
	printf "ACCed Entries:"
	for (x = 0; x <10; x++) {
		printf "%10d", ve[x]
	}
	printf "\n"
	#printf "Avg V Cycles :"
	#for (x = 0; x <10; x++) {
	#	if (ve[x] == 0)
	#		printf "       N/A"
	#	else
	#		printf "%10d", cycles[x] / ve[x]
	#}
	#printf "\n"
	printf "Walk  Entries:"
	for (x = 0; x <10; x++) {
		printf "%10d", te[x]
	}
	printf "\n"
	printf "Walk  Size   :"
	for (x = 0; x <10; x++) {
		if (te[x] > 512*1024)
			printf "%9dM", (te[x] / (512 * 1024)) * 4
		else
			printf "%9dK", (te[x] / 512) * 4
	}
	printf "\n"
	printf "Avg T Cycles :"
	for (x = 0; x <10; x++) {
		if (te[x] == 0)
			printf "       N/A"
		else
			printf "%10d", cycles[x] / te[x]
	}
	printf "\n"
	printf "\n"
}
