#!/bin/sh
sed -e "s@//#define MBED@#define MBED@g" mandelbrot.c > ./mbed/main.cpp
cd mbed
make
