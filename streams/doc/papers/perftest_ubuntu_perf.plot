set terminal fig
set output 'perftest_ubuntu_perf.fig'
set nologscale
set logscale x
set ylabel 'Write Rate (Writes per Second)'
set xlabel 'Write Size (Bytes)'
set grid x y
plot 'perftest_ubuntu.dat' using 1:($2) smooth unique title "LiS" with linespoints,\
     'perftest_ubuntu.dat' using 1:($3) smooth unique title "STREAMS" with linespoints,\
     'perftest_ubuntu.dat' using 1:($4) smooth unique title "Linux" with linespoints
