set datafile separator ','
set key autotitle columnhead # use first line as title

set ylabel "Time (us)"
set xlabel "Input string size"

set y2tics # enable second axis
set ytics nomirror # don't show tics on the left side
set y2label "Ranges-v3 / No Fancy time ratio"
set y2tics 0.5
set y2range [0:5]

set style line 100 lt 1 lc rgb "grey" lw 0.5
set grid ls 100
# set ytics 2e-6

set style line 101 lw 2 lt rgb "red"
set style line 102 lw 2 lt rgb "purple"
set style line 103 lw 1 lt rgb "olive"

set key left top

set title titel
plot filename using 1:2 with lines ls 101, '' using 1:4 with lines ls 102, '' using 1:6 with lines ls 103 axis x1y2
