set terminal fig color small portrait fontsize 9 inches size 4.5 2.4
set output 'perftest_rh7_comp.fig'
set nologscale
set logscale x
set ylabel 'Percentage Improvement (over Linux)'
set y2label '|'
set xlabel 'Write Size (Bytes)'
set grid x y
set key top right Left reverse
#set border 12 lw 0
#set border 3 lw 0.5
set border 3 lw 1
set xtics nomirror (1,2,4,8,16,32,64,128,256,512,1024,2048,4096)
set ytics nomirror
set format y "%3.0f%%"
set tics in
plot \
     'perftest_rh7.dat' using 1:(($3/$4-1)*100) smooth unique title "Streams" with linespoints lt 5,\
     'perftest_rh7.dat' using 1:(($4/$4-1)*100) smooth unique title "Linux" with linespoints lt 3,\
     'perftest_rh7.dat' using 1:(($2/$4-1)*100) smooth unique title "LiS" with linespoints lt 2
