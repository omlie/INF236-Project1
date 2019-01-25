#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string.h>
#include <iterator>
using namespace std;

int step(vector<char>& oldvec, vector<char>& newvec, map<string, char>& rules)
{
	oldvec = newvec;
	for (unsigned int i = 0; i < oldvec.size(); i++)
	{
		char fst, snd, trd;

		if (i == 0) fst = oldvec[oldvec.size() - 1];
		else fst = oldvec[i - 1];
		
		snd = oldvec[i];
		
		if (i == oldvec.size() - 1) trd = oldvec[0];
		else trd = oldvec[i + 1];

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
		cout << "Need more arguments."<<endl;
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

	char c;
	while (second_file >> c) 
	{
		oldconfig.push_back(c);	
	}

	vector<char>newconfig = oldconfig;
	bool all_iterations[time][config_length]; 

	for (int i = 0; i < time; i++)
	{

		for(unsigned int j = 0; j < newconfig.size(); ++j)
		{
			if(newconfig[j] == '1')
			{
				all_iterations[i][j] = true;
				cout << '#';
			}
			else
			{
				all_iterations[i][j] = false;
				cout << '-';
			}
		}
		cout << "" << endl;
		step(oldconfig, newconfig, transformation_rules);
	}


	return 0;
}