set terminal fig
set output 'perftest_nb_delay.fig'
set nologscale
set ylabel 'Delay (Seconds per Write)'
set xlabel 'Write Size (Bytes)'
set grid x y
plot 'perftest_nb.dat' using 1:(1/$2) smooth unique title "LiS" with linespoints,\
     'perftest_nb.dat' using 1:(1/$3) smooth unique title "STREAMS" with linespoints,\
     'perftest_nb.dat' using 1:(1/$4) smooth unique title "Linux" with linespoints
