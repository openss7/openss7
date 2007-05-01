set terminal fig
set output 'netperf_suse_perf.fig'
set nologscale
set logscale x
set ylabel 'Message Rate (Messages per Second)'
set xlabel 'Message Size (Bytes)'
set grid x y
plot 'netperf_suse.dat' using 1:($2/10) smooth unique title "XTIoS Tx" with linespoints,\
     'netperf_suse.dat' using 1:($3/10) smooth unique title "XTIoS Rx" with linespoints,\
     'netperf_suse.dat' using 1:($4/10) smooth unique title "XTI Tx" with linespoints,\
     'netperf_suse.dat' using 1:($5/10) smooth unique title "XTI Rx" with linespoints,\
     'netperf_suse.dat' using 1:($6/10) smooth unique title "Sockets Tx" with linespoints,\
     'netperf_suse.dat' using 1:($7/10) smooth unique title "Sockets Rx" with linespoints
