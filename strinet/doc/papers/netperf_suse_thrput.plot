set terminal fig
set output 'netperf_suse_thrput.fig'
set logscale
set ylabel 'Data Rate (Bits per Second)'
set xlabel 'Message Size (Bytes)'
set grid x y
set key right bottom Right noreverse
plot 'netperf_suse.dat' using 1:($1*$2*8/10) smooth unique title "XTIoS Tx" with linespoints,\
     'netperf_suse.dat' using 1:($1*$3*8/10) smooth unique title "XTIoS Rx" with linespoints,\
     'netperf_suse.dat' using 1:($1*$4*8/10) smooth unique title "XTI Tx" with linespoints,\
     'netperf_suse.dat' using 1:($1*$5*8/10) smooth unique title "XTI Rx" with linespoints,\
     'netperf_suse.dat' using 1:($1*$6*8/10) smooth unique title "Sockets Tx" with linespoints,\
     'netperf_suse.dat' using 1:($1*$7*8/10) smooth unique title "Sockets Rx" with linespoints
