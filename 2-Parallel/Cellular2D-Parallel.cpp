#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string.h>
#include <iterator>
using namespace std;


int malloc2dchar(char ***array, int n, int m) {

    /* allocate the n*m contiguous items */
    char *p = (char *)malloc(n*m*sizeof(char));
    if (!p) return -1;

    /* allocate the row pointers into the memory */
    (*array) = (char **)malloc(n*sizeof(char*));
    if (!(*array)) {
       free(p);
       return -1;
    }

    /* set up the pointers into the contiguous memory */
    for (int i=0; i<n; i++)
       (*array)[i] = &(p[i*m]);

    return 0;
}

int free2dchar(char ***array) {
    /* free the memory - the first element of the array is at the start */
    free(&((*array)[0][0]));

    /* free the pointers into the memory */
    free(*array);

    return 0;
}


int main(int argc, char** argv) {
	char **global, **local;
	int gridsize = 9;
	const int minsplit = 3;
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
	else{

	}
	MPI_Finalize();
	return 0;
}