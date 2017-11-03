# awk  -f bandwidth_calculation.awk user=weiyang bandwidth
BEGIN {inbound=0; outbound=0}

$0 ~ user {
	inbound += $3
	outbound += $4
}

END {print inbound "  " outbound}
