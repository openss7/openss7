set terminal fig
set output 'netperf_ubuntu_comp.fig'
set nologscale
set logscale x
set ylabel 'Percentage Improvement (over Sockets)'
set xlabel 'Message Size (Bytes)'
set grid x y
set key left bottom Left noreverse
plot 'netperf_ubuntu.dat' using 1:(($2/$6)*100) smooth unique title "XTIoS Tx" with linespoints,\
     'netperf_ubuntu.dat' using 1:(($3/$7)*100) smooth unique title "XTIoS Rx" with linespoints,\
     'netperf_ubuntu.dat' using 1:(($4/$6)*100) smooth unique title "XTI Tx" with linespoints,\
     'netperf_ubuntu.dat' using 1:(($5/$7)*100) smooth unique title "XTI Rx" with linespoints,\
     'netperf_ubuntu.dat' using 1:(($6/$6)*100) smooth unique title "Sockets Tx" with linespoints,\
     'netperf_ubuntu.dat' using 1:(($7/$7)*100) smooth unique title "Sockets Rx" with linespoints
