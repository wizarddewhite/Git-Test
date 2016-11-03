BEGIN {
	FS=":";
}

# process first file and create an array
NR==FNR {origin[$1]=$2; next}

FNR==1 {
	printf "%10s:    %10d\n", $1, origin[$1]
	printf "%10s     %10s     %10s     %10s\n", " ", "origin", "optimize", "percentage"
}

FNR!=1 {
	printf "%10s:    %10.3f    %10.3f    %10.2f\n", $1, origin[$1], $2, $2 / origin[$1]
}
