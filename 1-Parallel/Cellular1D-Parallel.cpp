/**
MIT License

Copyright (c) 2019 omlie

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
**/
#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <ctime>
#include <string.h>
#include <iterator>
using namespace std;

char *alloc_1d_char(int length)
{
	char *arr = (char *)malloc(length*sizeof(char));
	return arr;
}

int step(char *board, char recv_fst, char recv_trd, map<string, char>& rules, int length)
{
	char *oldboard = alloc_1d_char(length);
	copy(&board[0], &board[0]+length, &oldboard[0]);
	for (int i = 0; i < length; i++)
	{
		char fst, snd, trd = '0';
		if (i == 0)
			fst = recv_fst;
		else
			fst = oldboard[(i - 1) % length];
		if (i == length - 1)
			trd = recv_trd;
		else
			trd = oldboard[(i + 1) % length];

		snd = board[i];

		vector<char> nb= {fst, snd, trd};
        string result(nb.begin(), nb.end());
		board[i] = rules[result];
	}
	delete [] oldboard;
	return 0;
}

int main(int argc, char** argv) {
	double start_time = MPI_Wtime();
	int rank, size;
	
	MPI_Init(&argc, &argv);  //initialize MPI library
	MPI_Comm_size(MPI_COMM_WORLD, &size); //get number of processes
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); //get my process id

	int time, numrules, length;
	char *global_board, *local_board, tempfst, temptrd;
	vector<vector<char>> all_iterations;
	int *sizeof_subarray, *displacement;
	map<string, char> transformation_rules;
	int CONFIG = 1, STATE = 2, KEY = 3, VALUE = 4;

	if (rank == 0) 
	{
		string transformation_function_file, inital_config_file;

		if (argc == 4)
		{
			transformation_function_file = argv[1];
			inital_config_file = argv[2];
			time = atoi(argv[3]);
		}
		else
		{
			cout << "Need more arguments."<<endl;
			exit(0);
		}

		ifstream first_file(transformation_function_file);

		vector<string> keys;
		vector<char>  values;
		string s;
		while(getline(first_file, s))
		{
			keys.push_back(s.substr(0,3));
			values.push_back(s.at(s.length() - 1));	
		}

		numrules = keys.size();

		first_file.close();
		ifstream second_file(inital_config_file);

		getline(second_file, s);
		length = stoi(s);
		global_board = alloc_1d_char(length);

		char c;
		int i = 0;
		while (second_file >> c) 
		{
			global_board[i] = c;
			i++	;
		}
		MPI_Bcast(&length, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(&time, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(&numrules, 1, MPI_INT, 0, MPI_COMM_WORLD);

		for (int s = 1; s < size; s++)
		{
			for (int i = 0; i < numrules; i++)
			{
				string key = keys[i];
                char value = values[i];
                transformation_rules.insert(pair<string,char>(key, value));
                MPI_Send(key.c_str(), key.length(), MPI_CHAR, s, KEY, MPI_COMM_WORLD);
                MPI_Send(&value, 1, MPI_CHAR, s, VALUE, MPI_COMM_WORLD);
			}
		}
		
		all_iterations = {};
	}
	if (rank != 0)
	{
		MPI_Bcast(&length, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(&time, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(&numrules, 1, MPI_INT, 0, MPI_COMM_WORLD);

		for (int i = 0; i < numrules; i++)
		{
			MPI_Status status;
			MPI_Probe(0, KEY, MPI_COMM_WORLD, &status);
			int l;
			MPI_Get_count(&status, MPI_CHAR, &l);
			char *buf = new char[l];
			MPI_Recv(buf, l, MPI_CHAR, 0, KEY, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			string key(buf, l);
			delete [] buf;
			char value;
			MPI_Recv(&value, 1, MPI_CHAR, 0, VALUE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			transformation_rules.insert(pair<string,char>(key, value));
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);
	
	sizeof_subarray = (int *)malloc(size * sizeof(int));// ALLOCATED SIZE OF GLOBAL ARRAY
    displacement = (int *)malloc(size * sizeof(int));   // PLACEMENT OF ALLOCATION IN GLOBAL ARRAY 

    for (int thisrank = 0; thisrank < size; thisrank++)
    {
        sizeof_subarray[thisrank] = ((thisrank + 1)*(length)/(size) - (thisrank)*(length)/(size));
        displacement[thisrank] = (thisrank*length/size);
    }

    local_board = alloc_1d_char(sizeof_subarray[rank]);

	MPI_Scatterv(rank == 0 ? &global_board[0] : NULL, sizeof_subarray, displacement, MPI_CHAR, 
                 &local_board[0], sizeof_subarray[rank], MPI_CHAR, 0, MPI_COMM_WORLD);
	

	for (int s = 0; s < time; s++)
	{
		if(rank == 0){
			vector<char> data(global_board, global_board + length);
			all_iterations.push_back(data);
		}

		int LEFT, RIGHT;
		if (rank == 0)
			LEFT = size - 1;
		else
			LEFT = rank - 1;
		if (rank == size - 1)
			RIGHT = 0;
		else
			RIGHT = rank + 1;

		MPI_Send(&local_board[0], 1, MPI_BYTE, LEFT, STATE, MPI_COMM_WORLD);
		MPI_Send(&local_board[sizeof_subarray[rank] - 1], 1, MPI_BYTE, RIGHT, STATE, MPI_COMM_WORLD);
		MPI_Recv(&tempfst, 1, MPI_BYTE, LEFT, STATE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&temptrd, 1, MPI_BYTE, RIGHT, STATE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		step(local_board, tempfst, temptrd, transformation_rules, sizeof_subarray[rank]);
		
		MPI_Gatherv(&local_board[0], sizeof_subarray[rank], MPI_CHAR,
                    rank == 0 ? &global_board[0] : NULL, sizeof_subarray, displacement, MPI_CHAR,
                    0, MPI_COMM_WORLD);
	}
	
	double end_time = MPI_Wtime();
	MPI_Finalize(); //MPI cleanup
	
	if (rank==0){ 
		cout << end_time - start_time << endl;
		/*ofstream output_file("allIterations.txt");
		for (unsigned int i = 0; i < all_iterations.size(); i++)
		{
			for(unsigned int j = 0; j < all_iterations[i].size(); ++j)
				output_file << all_iterations[i][j];
			output_file  << endl;
		}	
		output_file.close();*/
	}
	return 0;
}
		
