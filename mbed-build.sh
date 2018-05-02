#!/bin/sh
sed -e "s@//#define MBED@#define MBED@g" mandelbrot.c | more > ./mbed/main.cpp
cd mbed
make
