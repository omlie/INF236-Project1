#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string.h>
#include <iterator>
using namespace std;



int main(int argc, char** argv) {
	int rank, size;
	
	MPI_Init(&argc, &argv);  //initialize MPI library
	MPI_Comm_size(MPI_COMM_WORLD, &size); //get number of processes
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); //get my process id

	int time, numrules;
	int CONFIG = 1, STATE = 2, KEY = 3, VALUE = 4;

	if (rank == 0) 
	{
		string transformation_function_file, inital_config_file;
		int time;

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
		vector<char> oldconfig;

		char c;
		while (second_file >> c) 
		{
			oldconfig.push_back(c);	
		}

		
		int allocated = oldconfig.size() / (size - 1);
		int remainder = oldconfig.size() % (size - 1);

		MPI_Bcast(&time, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(&numrules, 1, MPI_INT, 0, MPI_COMM_WORLD);

		for (int s = 1; s < size; s++)
		{
			for (int i = 0; i < numrules; i++)
			{
				string key = keys[i];
				char value = values[i];
				MPI_Send(key.c_str(), key.length(), MPI_CHAR, s, KEY, MPI_COMM_WORLD);
				MPI_Send(&value, 1, MPI_CHAR, s, VALUE, MPI_COMM_WORLD);
			}
		}

		vector<vector<char>> all_iterations = {oldconfig}; 

		for (int s = 0; s < time; s++)
		{

			for (int i = 0; i < size - 1; i++)
			{
				int msgsize = allocated;
				if (i == size - 2)
					msgsize = allocated + remainder;
				MPI_Send(&msgsize, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
				MPI_Send(&oldconfig[i * allocated], msgsize, MPI_BYTE, i + 1, CONFIG, MPI_COMM_WORLD);
			}
			vector<char> nextconfig;
			for (int i = 0; i < size - 1; i++)
			{
				std::vector<char> receive;
				int sz;
				MPI_Recv(&sz, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				receive.resize(sz);
				MPI_Recv(&receive[0], sz, MPI_BYTE, i + 1, CONFIG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				
				for (int x: receive)
					nextconfig.push_back(x);
			}
			
			oldconfig = nextconfig;
			all_iterations.push_back(oldconfig);
		}
		ofstream output_file("allIterations.txt");
	
		for (unsigned int i = 0; i < all_iterations.size(); i++)
		{
				for(unsigned int j = 0; j < all_iterations[i].size(); ++j)
					output_file << all_iterations[i][j];
				output_file << "" << endl;
		}
		
		output_file.close();
	}
	else
	{
		MPI_Bcast(&time, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(&numrules, 1, MPI_INT, 0, MPI_COMM_WORLD);
		map<string, char> transformation_rules;

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
		
		for (int s = 0; s < time; s++)
		{
			int msgsize, vecsize, LEFT, RIGHT;
			vector<char> fromconfig, toconfig;
			char fst, snd, trd, tempfst, temptrd;
			
			MPI_Recv(&msgsize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			fromconfig.resize(msgsize);
			MPI_Recv(&fromconfig[0], msgsize, MPI_BYTE, 0, CONFIG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		 	vecsize = fromconfig.size();
			toconfig = fromconfig;

			if ((rank - 1) == 0)
				LEFT = size - 1;
			else
				LEFT = rank - 1;
			if ((rank + 1) == size)
				RIGHT = 1;
			else
				RIGHT = rank + 1;

			
			MPI_Send(&fromconfig[0], 1, MPI_BYTE, LEFT, STATE, MPI_COMM_WORLD);
			MPI_Send(&fromconfig[vecsize - 1], 1, MPI_BYTE, RIGHT, STATE, MPI_COMM_WORLD);
			MPI_Recv(&tempfst, 1, MPI_BYTE, LEFT, STATE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&temptrd, 1, MPI_BYTE, RIGHT, STATE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			
			for (int i = 0; i < vecsize; i++)
			{
				if (i == 0)
					fst = tempfst;
				else
					fst = fromconfig[(i - 1) % vecsize];
				if (i == vecsize - 1)
					trd = temptrd;
				else
					trd = fromconfig[(i + 1) % vecsize];

				snd = fromconfig[i];

				string triple = "";
				triple += fst;
				triple += snd; 
				triple += trd;
			
				toconfig[i] = transformation_rules[triple];
			}

			int newsize = toconfig.size();
			MPI_Send(&newsize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
			MPI_Send(&toconfig[0], newsize, MPI_BYTE, 0, CONFIG, MPI_COMM_WORLD);

		}

	}
	MPI_Finalize(); //MPI cleanup
	return 0;
}
