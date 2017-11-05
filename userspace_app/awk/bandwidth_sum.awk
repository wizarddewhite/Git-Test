# awk  -f bandwidth_calculation.awk user=weiyang bandwidth
BEGIN {inbound=0; outbound=0}

{
	inbound += $1
	outbound += $2
}

END {print inbound "  " outbound}
