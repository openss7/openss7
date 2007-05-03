set terminal fig color small portrait fontsize 9 inches size 4.5 2.4
set output 'perftest_suse_thrput.fig'
set logscale
set ylabel 'Data Rate (Bits per Second)'
set y2label '|'
set xlabel 'Write Size (Bytes)'
set grid x y
set key bottom right Left reverse
#set border 12 lw 0
#set border 3 lw 0.5
set border 3 lw 1
set xtics nomirror (1,2,4,8,16,32,64,128,256,512,1024,2048,4096)
set ytics nomirror
set tics in
plot \
     'perftest_suse.dat' using 1:($1*$3*8) smooth unique title "Streams" with linespoints lt 5,\
     'perftest_suse.dat' using 1:($1*$4*8) smooth unique title "Linux" with linespoints lt 3,\
     'perftest_suse.dat' using 1:($1*$2*8) smooth unique title "LiS" with linespoints lt 2
