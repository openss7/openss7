set terminal fig color small portrait fontsize 9 inches size 4.5 2.4
set output 'netperf_smp_comp.fig'
set nologscale
set logscale x
set ylabel 'Percentage Improvement (over Sockets)'
set y2label '|'
set xlabel 'Message Size (Bytes)'
set grid x y
set key bottom left Right noreverse
set xtics nomirror (1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192)
set ytics nomirror
set border 3 lw 1
plot \
     'netperf_smp.dat' using 1:(($4/$6)*100) smooth unique title "Streams Tx" with linespoints lt 6,\
     'netperf_smp.dat' using 1:(($5/$7)*100) smooth unique title "Streams Rx" with linespoints lt 5,\
     'netperf_smp.dat' using 1:(($6/$6)*100) smooth unique title "Sockets Tx" with linespoints lt 4,\
     'netperf_smp.dat' using 1:(($7/$7)*100) smooth unique title "Sockets Rx" with linespoints lt 3,\
     'netperf_smp.dat' using 1:(($2/$6)*100) smooth unique title "XTIoS Tx"   with linespoints lt 1,\
     'netperf_smp.dat' using 1:(($3/$7)*100) smooth unique title "XTIoS Rx"   with linespoints lt 2
