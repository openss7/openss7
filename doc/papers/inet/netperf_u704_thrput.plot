set terminal fig color small portrait fontsize 9 inches size 4.5 2.4
set output 'netperf_u704_thrput.fig'
set logscale
set ylabel 'Data Rate (Bits per Second)'
set y2label '|'
set xlabel 'Message Size (Bytes)'
set grid x y
set key bottom right Left reverse
set xtics nomirror (1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192)
set ytics nomirror
set border 3 lw 1
plot \
     'netperf_u704.dat' using 1:($1*$4*8/10) smooth unique title "Streams Tx" with linespoints lt 6,\
     'netperf_u704.dat' using 1:($1*$5*8/10) smooth unique title "Streams Rx" with linespoints lt 5,\
     'netperf_u704.dat' using 1:($1*$6*8/10) smooth unique title "Sockets Tx" with linespoints lt 4,\
     'netperf_u704.dat' using 1:($1*$7*8/10) smooth unique title "Sockets Rx" with linespoints lt 3,\
     'netperf_u704.dat' using 1:($1*$2*8/10) smooth unique title "XTIoS Tx"   with linespoints lt 1,\
     'netperf_u704.dat' using 1:($1*$3*8/10) smooth unique title "XTIoS Rx"   with linespoints lt 2
