#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;

int main(int argc, char const *argv[])
{
	int k = atoi(argv[1]);
	int p = pow(2, k);
	string str = "";
	for (int i = 0; i < p; i++)
		str += '0';
	str += '1';
	for (int i = 0; i < p - 1; i++)
		str += '0';
	string filename = "middle"; 
	filename += argv[1];
	filename += ".txt";
	ofstream output_file(filename);
	output_file << 2*p << endl;
	output_file << str << endl;
	return 0;
}