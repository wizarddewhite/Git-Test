BEGIN {
	FS="[ \tms]";
	real=0; user=0; sys=0;
	round=0;
}

/real/ {real+=$2 * 60 + $3}
/user/ {user+=$3 * 60 + $4}
/sys/ {sys+=$4 * 60 + $5}
/Round/ {round++}

END {
	printf "Total Round: %d\n", round
	printf "avg real:      %10.3f\n", real / round
	printf "avg user:      %10.3f\n", user / round
	printf "avg sys :      %10.3f\n", sys / round
}
