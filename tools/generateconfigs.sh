#!/bin/bash


g++ -std=c++11 generateinput.cpp -o generateinput.o
g++ -std=c++11 generategameoflife.cpp -o generategameoflife.o

k=( 10 11 12 13 14 15 16 17 18 19 20 )
N=( 1024 2048 4096 8192 16384 32768 65536 )


for i in "${k[@]}"
do
	./generateinput.o "$i"
done

for i in "${N[@]}"
do
	./generategameoflife.o "$i"
done
