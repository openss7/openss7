set terminal fig
set output 'perftest_suse_comp.fig'
set nologscale
set logscale x
set ylabel 'Percentage Improvement (over Linux)'
set xlabel 'Write Size (Bytes)'
set grid x y
plot 'perftest_suse.dat' using 1:(($2/$4)*100) smooth unique title "LiS" with linespoints,\
     'perftest_suse.dat' using 1:(($3/$4)*100) smooth unique title "STREAMS" with linespoints,\
     'perftest_suse.dat' using 1:(($4/$4)*100) smooth unique title "Linux" with linespoints
