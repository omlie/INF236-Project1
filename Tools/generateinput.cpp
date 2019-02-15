#include <iostream>
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
	cout << p*p << endl;
	cout << str << endl;
	return 0;
}