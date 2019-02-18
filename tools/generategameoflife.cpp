#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;

int main(int argc, char const *argv[])
{
	int k = atoi(argv[1]);
	string str = "gameoflife";
	str += argv[1];
	str += ".txt";
	ofstream output_file(str);
	output_file << k << endl;
	for (int i = 0; i < k; i++)
	{
		for(int j = 0; j < k; j++){
			char c = ((rand() % 2  + 1) == 1) ? '0' : '1';
			output_file << c;
		}
		output_file << endl;
	}
		
	output_file.close();
	
	return 0;
}

