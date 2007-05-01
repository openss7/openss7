set terminal fig
set output 'perftest_ubuntu_thrput.fig'
set logscale
set ylabel 'Data Rate (Bits per Second)'
set xlabel 'Write Size (Bytes)'
set grid x y
set key right bottom Right noreverse
plot 'perftest_ubuntu.dat' using 1:($1*$2*8) smooth unique title "LiS" with linespoints,\
     'perftest_ubuntu.dat' using 1:($1*$3*8) smooth unique title "STREAMS" with linespoints,\
     'perftest_ubuntu.dat' using 1:($1*$4*8) smooth unique title "Linux" with linespoints
