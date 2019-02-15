#!/bin/bash

#module load mpi/openmpi-x86_64
mpiCC -std=c++11 ../1-Parallel/Cellular1D-Parallel.cpp -O3 -o 1parallel.o
#mpiCC -std=c++11 ../2-Parallel/Cellular2D-Parallel.cpp -O3 -o 2parallel.o
g++ -std=c++11 generateinput.cpp -o generateinput.o

rm results1D.txt
echo Running parallel 1D automata

k=( 10 11 12 13 14 15 16 17 18 19 20 )
N=( 1024 2048 4096 8192 16384 32768 65536 )
processes=( 2 4 8 )  #16 32 64 128 256 516 ) 

printf "\t\t %s \t" "${processes[@]}"  >> results1D.txt
for i in "${k[@]}"
do
	rm middle"$i".txt
	s=$(./generateinput.o "$i")
	echo "$s" >> middle"$i".txt
	declare -a result
	result=()
	for p in "${processes[@]}"
	do
		result=("${result[@]}" $(mpirun -np "$p" ./1parallel.o mod2.txt middle"$i".txt 10))
	done
	printf "\n $i" >> results1D.txt
	printf "\t %s" "${result[@]}" >> results1D.txt
done
