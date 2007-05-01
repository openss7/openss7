set terminal fig
set output 'netperf_rh7_delay.fig'
set nologscale
set ylabel 'Delay (Seconds per Message)'
set xlabel 'Message Size (Bytes)'
set grid x y
set key left top Left noreverse
plot 'netperf_rh7.dat' using 1:(10/$2) smooth unique title "XTIoS Tx" with linespoints,\
     'netperf_rh7.dat' using 1:(10/$3) smooth unique title "XTIoS Rx" with linespoints,\
     'netperf_rh7.dat' using 1:(10/$4) smooth unique title "XTI Tx" with linespoints,\
     'netperf_rh7.dat' using 1:(10/$5) smooth unique title "XTI Rx" with linespoints,\
     'netperf_rh7.dat' using 1:(10/$6) smooth unique title "Sockets Tx" with linespoints,\
     'netperf_rh7.dat' using 1:(10/$7) smooth unique title "Sockets Rx" with linespoints
