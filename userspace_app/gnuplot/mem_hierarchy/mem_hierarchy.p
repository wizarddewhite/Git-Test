set terminal jpeg giant size 1680,1050
set output "mem_hirarchy.jpeg"
set logscale x
set xtics rotate
plot 'cache_plot.txt' using 1:2:xtic(1) w steps
