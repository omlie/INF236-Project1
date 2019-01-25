#include <mpi.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <iterator>
using namespace std;

int step(vector<char>& oldvec, vector<char>& newvec, map<string, char>& rules)
{
	oldvec = newvec;
	int vecsize = oldvec.size();
	for (unsigned int i = 0; i < vecsize; i++)
	{
		char fst, snd, trd;

		if (i == 0) fst = oldvec[vecsize - 1];
		else fst = oldvec[(i - 1) % vecsize];
		
		snd = oldvec[i];
		
		if (i == vecsize - 1) trd = oldvec[0];
		else trd = oldvec[(i + 1) % vecsize];

		string triple ="";
		triple += fst;
		triple += snd;
		triple += trd;

		newvec[i] = rules[triple];
		
	}

	return 0;
}

int main(int argc, char** argv)
{
	int comm_sz;
	int my_rank;
	std::map<string, char> transformation_rules;
	map<string,char>::iterator itr;


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
		cout << "Need more arguments."<< endl;
		exit(0);
	}


	ifstream first_file(transformation_function_file);

	string s;
	while(getline(first_file, s))
	{
		string key = s.substr(0,3);
		char val = s.at(s.length() - 1);
		transformation_rules.insert(pair<string, char>(key, val));
	}

	first_file.close();
	ifstream second_file(inital_config_file);

	getline(second_file, s);
	int config_length = stoi(s);
	vector<char> oldconfig;
	cout << config_length << endl;
	char c;
	while (second_file >> c) 
	{
		oldconfig.push_back(c);	
	}

	vector<char>newconfig = oldconfig;
	//bool all_iterations[time][config_length]; 

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	for (int i = 0; i < time; i++)
	{

		for(unsigned int j = 0; j < newconfig.size(); ++j)
		{
			if(newconfig[j] == '1')
			{
				//all_iterations[i][j] = true;
				cout << '#';
			}
			else
			{
				//all_iterations[i][j] = false;
				cout << '-';
			}
		}
		cout << "" << endl;
		step(oldconfig, newconfig, transformation_rules);
	}


	return 0;
}