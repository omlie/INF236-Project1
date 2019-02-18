/*
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
*/
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
			a = b = c = d = f = g = h = i = '0',

			current = oldvec[row][col];

			if(row != 0 && col != 0)
				a = oldvec[row - 1][col - 1];
			
			if(row != 0 && col != vecsize - 1)
				c = oldvec[row - 1][col + 1];
			
			if (row != vecsize - 1 && col != 0)
				g = oldvec[row + 1][col - 1];
			
			if (row != vecsize - 1 && col != vecsize - 1)
				i = oldvec[row + 1][col + 1];
			
			if (row != 0)
				b = oldvec[row - 1][col];
			
			if (row != vecsize - 1)
				h = oldvec[row + 1][col];
			
			if (col != 0)
				d = oldvec[row][col - 1];
			
			if (col != vecsize - 1)
				f = oldvec[row][col + 1];
			
			vector<char> nb= {a, b, c, d, current, f, g, h, i};
			string result(nb.begin(), nb.end());
			
			newvec[row][col] = rules[result];
			
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
	while(!first_file.eof())
	{
		getline(first_file, s);
		string key = s.substr(0,9);
		char val = s.at(s.length() - 1);
		transformation_rules.insert(pair<string, char>(key, val));
	}

	first_file.close();
	ifstream second_file(inital_config_file);
	getline(second_file, s);
	int config_length = stoi(s);
	vector<vector<char>> oldconfig;

	while (!second_file.eof()){
		getline(second_file, s);
		vector<char> data(s.begin(), s.end());
		oldconfig.push_back(data);
	}
	second_file.close();

	
	vector<vector<char>> newconfig = oldconfig;
	vector<vector<vector<char>>> all_iterations = {newconfig}; 

	for (int i = 0; i < time; i++)
	{
		step(oldconfig, newconfig, transformation_rules, config_length);
		all_iterations.push_back(newconfig);
	}
	
	for (vector<char> vec : newconfig){
		for (char c : vec)
			cout << c;
		cout << endl;
	}
	
	ofstream output_file("allIterations.txt");
	
	for (unsigned int i = 0; i < all_iterations.size(); i++)
	{
			for(unsigned int j = 0; j < all_iterations[i].size(); ++j){
				for (char c : all_iterations[i][j])
					output_file << c;
				output_file << endl;
			}
	}
	
	output_file.close();
	return 0;
}