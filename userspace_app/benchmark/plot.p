#!/usr/bin/gnuplot
set terminal jpeg giant size 1680,1050
set output "mem_hierarchy.jpeg"
set logscale x
set xtics rotate
plot 'mem_hi.txt' using 1:2:xtic(1) w steps