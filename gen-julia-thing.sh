#!/bin/bash

STEP=0.007
ITER=700
XRES=5760
YRES=3240
XRESF=1920
YRESF=1080

PI=3.14159265358979323844

function msin-rad {
	echo "s($1)" | bc -l
}

function mcos-rad {
	echo "c($1)" | bc -l
}

function math {
	echo "$1" | bc -l
}

mkdir -p ./test
gcc -lm mandelbrot.c -o mandelbrot

N=1;r=0.7885;for a in $(seq 0 $STEP 2); do ./mandelbrot ./test/p$N.ppm $XRES $YRES $ITER 0 0 0 $(math "$r * `mcos-rad $(math $a*$PI)`") $(math "$r * `msin-rad $(math $a*$PI)`"); convert -resize "${XRESF}x$YRESF" ./test/p$N.ppm ./test/p$N-$a.jpg; rm ./test/p$N.ppm; N=$(expr $N + 1); echo "done #$N: a=$a"; done

