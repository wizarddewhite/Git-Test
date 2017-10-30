# awk  -f bandwidth_calculation.awk user=weiyang bandwidth
BEGIN {bandwidth = 0}

$0 ~ user {
	bandwidth += ($3 + $4) * 5
}

END {print bandwidth}
