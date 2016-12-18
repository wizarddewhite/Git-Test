#!/bin/bash
if [ $# -eq 0 ]
then
	echo "$0 data1[data2 ..]"
	exit
fi

GNUPLOT_CMD="set terminal jpeg giant size 1680,1050; set output \"compound.jpeg\";"

GNUPLOT_PLT=""

for FILE in $@; do
	if [[ -z $GNUPLOT_PLT ]]
	then
		GNUPLOT_PLT="plot '${FILE}' with lines "
	else
		GNUPLOT_PLT=$GNUPLOT_PLT", '${FILE}' with lines"
	fi
done

echo $GNUPLOT_CMD $GNUPLOT_PLT | gnuplot
echo $GNUPLOT_CMD $GNUPLOT_PLT 

