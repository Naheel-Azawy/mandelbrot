#!/bin/bash
X1=5760
Y1=3240
X2=1920
Y2=1080

PI=3.14159265358979323844
function sin { echo "s($1)" | bc -l; }
function cos { echo "c($1)" | bc -l; }
function math { echo "$@" | bc -l; }

./mandelbrot wall.ppm $X1 $Y1 1200 10.3 0.270925 0.004747
convert -resize ${X2}x${Y2} wall.ppm wall.png && rm wall.ppm

r=0.7885 && ./mandelbrot wall2.ppm $X1 $Y1 700 0.25 0 0 $(math "$r * `cos $(math 0.945*$PI)`") $(math "$r * `sin $(math 0.945*$PI)`") && convert -resize ${X2}x${Y2} wall2.ppm wall2.png && rm -f wall2.ppm
