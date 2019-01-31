#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string.h>
#include <iterator>
using namespace std;

int step(vector<vector<char>>& oldvec, vector<vector<char>>& newvec, map<string, char>& rules, int& vecsize)
{
	oldvec = newvec;
	for (int row = 0; row < vecsize; row++)
	{
		for (int col = 0; col < vecsize; col++)
		{
			char a, b, c, d, current, f, g, h, i;

			current = oldvec[row][col];

			if(row != 0 && col != 0)
			{
				a = oldvec[row - 1][col - 1];
				c = oldvec[row - 1][col + 1];
			}
			else
			{
				a = 0;
				c = 0;
			}

			if (row != vecsize - 1 && col != vecsize - 1)
			{
				g = oldvec[row + 1][col - 1];
				i = oldvec[row + 1][col + 1];
			}
			else{

				g = 0;
				i = 0;
			}

			if (row != 0)
				b = oldvec[row - 1][col];
			if (row != vecsize - 1)
				h = oldvec[row + 1][col];
			if (col != 0)
				d = oldvec[row][col - 1];
			if (col != vecsize - 1)
				f = oldvec[row][col + 1];

			string result = "" + a + b + c + d + current + f + g + h + i;

			newvec[i] = rules[result];
			
		}
		
	}

	return 0;
}



int main(int argc, char** argv)
{
	std::map<string, char> transformation_rules;

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
	vector<vector<char>> oldconfig;

	for (int row = 0; row < config_length; row++){
		for (int col = 0; col < config_length; col++){
			second_file >> oldconfig[row][col];
		}
	}

	vector<vector<char>> newconfig = oldconfig;
	vector<vector<vector<char>>> all_iterations = {newconfig}; 

	for (int i = 0; i < time; i++)
	{
		step(oldconfig, newconfig, transformation_rules, config_length);
		all_iterations.push_back(newconfig);
	}
	
	ofstream output_file("allIterations.txt");
	
	for (unsigned int i = 0; i < all_iterations.size(); i++)
	{
			for(unsigned int j = 0; j < all_iterations[i].size(); ++j)
				output_file << all_iterations[i][j][j];
			output_file << "" << endl;
	}
	
	output_file.close();
	return 0;
}