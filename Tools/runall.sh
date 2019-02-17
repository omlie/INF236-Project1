#!/bin/bash

module load mpi/openmpi-x86_64
mpiCC -std=c++11 1D-Parallel.cpp -O3 -o 1parallel.o
mpiCC -std=c++11 2D-Parallel.cpp -O3 -o 2parallel.o

mkdir output

k=( 10 11 12 13 14 15 16 17 18 19 20 )
N=( 1024 2048 4096 8192 16384 32768 ) #65536 )
processes=( 2 4 8 16 32 64 128 256 512 ) 

echo Running parallel 1D automata
printf "%15s" "${processes[@]}"  >> output/results1D.txt
for i in "${k[@]}"
do
	result=()
	for p in "${processes[@]}"
	do
		echo Running with size "$i" and "$p" processes
		result=("${result[@]}" $(mpiexec -np "$p" ./1parallel.o rules/mod2.txt input/middle"$i".txt 1000000))
	done
	printf "\n $i" >> output/results1D.txt
	printf "%15s" "${result[@]}" >> output/results1D.txt
done


echo Running parallel 2D automata
printf "%15s" "${processes[@]}"  >> output/results2D.txt
for i in "${N[@]}"
do
	result=()
	for p in "${processes[@]}"
	do
		echo Running with size "$i" and "$p" processes
		result=("${result[@]}" $(mpiexec -np "$p" ./2parallel.o rules/gameoflife.txt input/gameoflife"$i".txt 10))
	done
	printf "\n $i" >> output/results2D.txt
	printf "%15s" "${result[@]}" >> output/results2D.txt
done
