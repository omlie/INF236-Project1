#include "mpi.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include <ctime>
#include <string.h>
#include <iterator>
using namespace std;

/** ALLOCATION OF 2D ARRAY **/
char **alloc_2d_char(int rows, int cols) 
{
    char *data = (char *)malloc(rows*cols*sizeof(char));
    char **array= (char **)malloc(rows*sizeof(char*));
    
    for (int i = 0; i < rows; i++)
        array[i] = &(data[cols*i]);

    return array;
}


/** DEALLOCATION OF 2D ARRAY **/
void destroy_2d_char(char **arr, int rows)
{
  delete [] arr[0];
  delete [] arr;
}


/** GAME LOGIC **/
int step(char** board, char** newboard, map<string, char>& rules, int& height, int& width)
{
    for (int row = 1; row < height + 1; row++)
    {
        for (int col = 0; col < width; col++)
        {
            char a, b, c, d, current, f, g, h, i;
            a = b = c = d = f = g = h = i = '0',

            current = newboard[row - 1][col];

            if(col != 0){
                a = board[row - 1][col - 1];
                d = board[row][col - 1];
                g = board[row + 1][col - 1];
            }
            
            if(col != width - 1){
                c = board[row - 1][col + 1];
                i = board[row + 1][col + 1];
                f = board[row][col + 1];
            }
            
            b = board[row - 1][col];
            h = board[row + 1][col];
        
            vector<char> nb= {a, b, c, d, current, f, g, h, i};
            string result(nb.begin(), nb.end());
            
            newboard[row - 1][col] = rules[result];
        }
    }

    return 0;
}

/** MAIN PROGRAM - MPI LOGIC**/
int main(int argc, char** argv)
{
    double start_time = MPI_Wtime();
    int rank, size;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int time, numrules, dimension;
    map<string, char> transformation_rules;
    char **global_board, **local_board, **board_with_neighbours;
    int *sizeof_subarray, *displacement;
    int KEY = 1, VALUE = 2, FROMUPTODOWN = 3, FROMDOWNTOUP = 4;

    /** MASTER PROCESS **/
    if (rank == 0) 
    {
        /** READING INPUT AND BROADCASTING TO CHILD PROCESSES **/
        string transformation_function_file, inital_config_file;

        if (argc == 4)
        {
            transformation_function_file = argv[1];
            inital_config_file = argv[2];
            time = atoi(argv[3]);
        }
        else
        {
            cout << "Need more arguments: <executable> <game-rules> <inital-configuration> <steps>" << endl;
            MPI_Abort(MPI_COMM_WORLD, 0);
            exit(0);
        }

        /** READ GAME RULES **/
        ifstream first_file(transformation_function_file);

        vector<string> keys;
        vector<char>  values;
        string s;
        while(getline(first_file, s))
        {
            keys.push_back(s.substr(0,9));
            values.push_back(s.at(s.length() - 1)); 
        }

        numrules = keys.size();

        first_file.close();
        
        /** READ INITIAL CONFIGURATION INTO ARRAY **/
        ifstream second_file(inital_config_file);
        getline(second_file, s);

        dimension = stoi(s);
        
        global_board = alloc_2d_char(dimension, dimension);
        
        int row = -1;
        while (!second_file.eof())
        {  
            row += 1;
            getline(second_file, s);
            int col = -1;
            for ( std::string::iterator it=s.begin(); it!=s.end(); ++it){
                col += 1;
                global_board[row][col] = *it;
            }
        }
        second_file.close();

        /** EXIT IF NUMBER OF PROCESSES EXCEEDS DIMENSION **/
        if (size > dimension)
        {
            cout << "Number of processes cannot exceed the dimension of the input.\nDimension: " 
            << dimension << "\nProcesses: " << size << endl;
            MPI_Abort(MPI_COMM_WORLD, 0);
            exit(0);
        }

        /** BROADCAST DIMENSION, NUMBER OF STEPS AND NUMBER OF RULES**/
        MPI_Bcast(&dimension, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&time, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&numrules, 1, MPI_INT, 0, MPI_COMM_WORLD);


        /** SEND RULES FROM MASTER TO CHILDREN **/
        for (int s = 1; s < size; s++)
        {
            for (int i = 0; i < numrules; i++)
            {
                string key = keys[i];
                char value = values[i];
                transformation_rules.insert(pair<string,char>(key, value));
                MPI_Send(key.c_str(), key.length(), MPI_CHAR, s, KEY, MPI_COMM_WORLD);
                MPI_Send(&value, 1, MPI_CHAR, s, VALUE, MPI_COMM_WORLD);
            }
        }
    }

    /** CHILDREN RECEIVE DATA FROM MASTER PROCESS **/
    if (rank != 0)
    {
        MPI_Bcast(&dimension, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&time, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&numrules, 1, MPI_INT, 0, MPI_COMM_WORLD);

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
    }

    sizeof_subarray = (int *)malloc(size * sizeof(int));// ALLOCATED SIZE OF GLOBAL ARRAY
    displacement = (int *)malloc(size * sizeof(int));   // PLACEMENT OF ALLOCATION IN GLOBAL ARRAY 

    for (int thisrank = 0; thisrank < size; thisrank++)
    {
        sizeof_subarray[thisrank] = ((thisrank + 1)*(dimension)/(size) - (thisrank)*(dimension)/(size))*dimension;
        displacement[thisrank] = (thisrank*dimension/size)*dimension;
    }

    int numrows = sizeof_subarray[rank]/dimension; //NUMBER OF ROWS ALLOCATED

    local_board = alloc_2d_char(numrows, dimension); // ARRAY TO PROCESS

    /**SCATTER GLOBAL ARRAY INTO EACH PROCESS' LOCAL ARRAY**/
    MPI_Scatterv(rank == 0 ? &global_board[0][0] : NULL, sizeof_subarray, displacement, MPI_CHAR, 
                &local_board[0][0], sizeof_subarray[rank], MPI_CHAR, 0, MPI_COMM_WORLD);
    
    /** UPDATE BOARD N TIMES **/
    for (int t = 0; t < time; t++){


        /** NEIGHBOUR PROCESSES **/
        int UP = rank == 0 ? size - 1 : rank - 1, 
            DOWN = rank == size - 1 ? 0 : rank + 1;
        
        /** ROWS TO SEND **/
        int first_row = 0, 
            second_row = numrows - 1;
        
        /** RECEIVE/SEND BUFFERS **/
        char *fromup = (char*)malloc(dimension*sizeof(char)), 
             *fromdown = (char*)malloc(dimension*sizeof(char)),
             *toup = (char*)malloc(dimension*sizeof(char)),
             *todown = (char*)malloc(dimension*sizeof(char));

        for (int j = 0; j < dimension; j++)
        {
           toup[j] = local_board[first_row][j];
           todown[j] = local_board[second_row][j];
        }
        
        MPI_Request req;
        MPI_Isend(&toup[0], dimension, MPI_BYTE, UP, FROMDOWNTOUP, MPI_COMM_WORLD, &req);
        MPI_Isend(&todown[0], dimension, MPI_BYTE, DOWN, FROMUPTODOWN, MPI_COMM_WORLD, &req);
        MPI_Recv(&fromup[0], dimension, MPI_BYTE, UP, FROMUPTODOWN, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&fromdown[0], dimension, MPI_BYTE, DOWN, FROMDOWNTOUP, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Request_free(&req);

        /** CREATE BOARD WITH LOCAL ARRAY AND RECEIVED ROWS **/
        board_with_neighbours = alloc_2d_char(numrows + 2, dimension);
        copy(&local_board[0][0], &local_board[0][0]+numrows*dimension, &board_with_neighbours[1][0]);

        for (int row = 0; row < numrows + 2; row++)
        {
            for (int col = 0; col < dimension; col++)
            {
                if (row == 0)
                    board_with_neighbours[row][col] = fromup[col];
                else if (row == numrows + 1)
                    board_with_neighbours[row][col] = fromdown[col];
            }
        }
        
        /** GAME LOGIC ON LOCAL ARRAY**/
        step(board_with_neighbours, local_board, transformation_rules, numrows, dimension);
    }

    /** UPDATE GLOBAL ARRAY WITH PROCESSED LOCAL ARRAYS**/
    MPI_Gatherv(&local_board[0][0], sizeof_subarray[rank], MPI_CHAR,
                rank == 0 ? &global_board[0][0] : NULL, sizeof_subarray, displacement, MPI_CHAR,
                0, MPI_COMM_WORLD);

    /** PRINT CONFIGURATION AFTER N STEPS **/
    if (rank == 0)
    {
        for (int i = 0; i < dimension; i++)
        {
            for (int j = 0; j < dimension; j++)
                cout << global_board[i][j];
            cout << endl;
        }
    }

    
    /** DEALLOCATE MEMORY OF USED ARRAYS **/
    destroy_2d_char(local_board, numrows);
    destroy_2d_char(board_with_neighbours, numrows + 2);
    if (rank == 0) destroy_2d_char(global_board, dimension);

    MPI_Finalize();
    double end_time = MPI_Wtime();
    if (rank == 0) cout << end_time - start_time <<  endl;
    return 0;
}