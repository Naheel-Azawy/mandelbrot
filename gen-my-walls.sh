#!/bin/bash
X1=5760
Y1=3240
X2=1920
Y2=1080

PI=3.14159265358979323844
function sin { echo "s($1)" | bc -l; }
function cos { echo "c($1)" | bc -l; }
function math { echo "$@" | bc -l; }

function resize {
    convert -resize ${X2}x${Y2} $1.ppm $1.png && rm -f $1.ppm
}

function w1 {
    ./mandelbrot wall1.ppm $X1 $Y1 1200 10.3 0.270925 0.004747
    resize wall1
}

function w2 {
    r=0.7885
    ./mandelbrot wall2.ppm $X1 $Y1 700 0.25 0 0 \
                 $(math "$r * `cos $(math 0.945*$PI)`") \
                 $(math "$r * `sin $(math 0.945*$PI)`")
    resize wall2
}

function w3 {
    ./mandelbrot wall3.ppm $X1 $Y1 200 0.25 0 0 -0.835 -0.2321
    resize wall3
}

function w4 {
    ./mandelbrot wall4.ppm $X1 $Y1 3000 12 -0.743643135 0.131825963
    resize wall4
}

w1
w2
w3
w4
