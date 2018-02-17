#!/bin/bash
gcc -lm mandelbrot.c -o mandelbrot
./mandelbrot wall.ppm 5760 3240 1200 10.3 0.270925 0.004747
convert -resize 1920x1080 wall.ppm wall.png
rm wall.ppm
