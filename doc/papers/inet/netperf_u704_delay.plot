set terminal fig color small portrait fontsize 9 inches size 4.5 2.4
set output 'netperf_u704_delay.fig'
set nologscale
set ylabel 'Delay (Seconds per Message)'
set y2label 'Delay'
set xlabel 'Message Size (Bytes)'
set grid x y
set key top left Right noreverse
set xtics nomirror (2048,4096,8192,16384)
set ytics nomirror
set border 3 lw 1
plot \
     'netperf_u704.dat' using 1:(10/$4) smooth unique title "Streams Tx" with linespoints lt 6,\
     'netperf_u704.dat' using 1:(10/$5) smooth unique title "Streams Rx" with linespoints lt 5,\
     'netperf_u704.dat' using 1:(10/$6) smooth unique title "Sockets Tx" with linespoints lt 4,\
     'netperf_u704.dat' using 1:(10/$7) smooth unique title "Sockets Rx" with linespoints lt 3,\
     'netperf_u704.dat' using 1:(10/$2) smooth unique title "XTIoS Tx"   with linespoints lt 1,\
     'netperf_u704.dat' using 1:(10/$3) smooth unique title "XTIoS Rx"   with linespoints lt 2
