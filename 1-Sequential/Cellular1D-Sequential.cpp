#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string.h>
#include <iterator>
using namespace std;

int step(vector<char>& oldvec, vector<char>& newvec, map<string, char>& rules, int& vecsize)
{
	oldvec = newvec;
	for (int i = 0; i < vecsize; i++)
	{
		char fst, snd, trd;

		if (i == 0) fst = oldvec[vecsize - 1];
		else fst = oldvec[(i - 1) % vecsize];
		
		snd = oldvec[i];
		
		if (i == vecsize - 1) trd = oldvec[0];
		else trd = oldvec[(i + 1) % vecsize];

		vector<char> nb = {fst, snd, trd};
		string triple(nb.begin(), nb.end());

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

	vector<char> newconfig = oldconfig;
	vector<vector<char>> all_iterations = {newconfig}; 

	for (int i = 0; i < time; i++)
	{
		step(oldconfig, newconfig, transformation_rules, config_length);
		all_iterations.push_back(newconfig);
	}

	for (char i : newconfig)
		cout << i;
	cout << endl;
	
	ofstream output_file("allIterations.txt");
	
	for (unsigned int i = 0; i < all_iterations.size(); i++)
	{
			for(unsigned int j = 0; j < all_iterations[i].size(); ++j)
				output_file << all_iterations[i][j];
			output_file << "" << endl;
	}
	
	output_file.close();
	return 0;
}