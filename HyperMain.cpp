#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <typeinfo>
#include <cstdio>
#include <chrono>
#include <random>
#include <climits>

//#include "Neighbours.h"
#include "VectorElement.h"
#include "HyperCube.h"
#include "IdDistancePair.h"
#include "Helpers.h"

using namespace std;

int main(int argc, char *argv[])
{

    //Setting up the algorithm parameters
    string FILE_NAME_INPUT = "NONE";
    string FILE_NAME_QUERY = "NONE";
    string FILE_NAME_LOG = "NONE";
    int k = -1;
    int M = -1;
    int probes = -1;
    int N = -1;
    double RANGE = -1.0;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-i") == 0)
        {
            FILE_NAME_INPUT = argv[i + 1];
        }
        else if (strcmp(argv[i], "-q") == 0)
        {
            FILE_NAME_QUERY = argv[i + 1];
        }
        else if (strcmp(argv[i], "-k") == 0)
        {
            string tempK;
            tempK = argv[i + 1];
            k = args_string_to_int(tempK);
        }
        else if (strcmp(argv[i], "-M") == 0)
        {
            string tempM;
            tempM = argv[i + 1];
            M = args_string_to_int(tempM);
        }
        else if (strcmp(argv[i], "-probes") == 0)
        {
            string tempProbes;
            tempProbes = argv[i + 1];
            probes = args_string_to_int(tempProbes);
        }
        else if (strcmp(argv[i], "-o") == 0)
        {
            FILE_NAME_LOG = argv[i + 1];
        }
        else if (strcmp(argv[i], "-N") == 0)
        {
            string tempN;
            tempN = argv[i + 1];
            N = args_string_to_int(tempN);
        }
        else if (strcmp(argv[i], "-R") == 0)
        {
            string tempR;
            tempR = argv[i + 1];
            RANGE = args_string_to_double(tempR);
        }
    }

    //If no file is provided the program will provide command prompts for the user to enter.
    if (FILE_NAME_INPUT == "NONE" && FILE_NAME_QUERY == "NONE" && FILE_NAME_LOG == "NONE")
    {
        string input;
        cout << "Please specify input file" << endl;
        cin >> input;
        FILE_NAME_INPUT = input;
        cout << "Please specify query file" << endl;
        cin >> input;
        FILE_NAME_QUERY = input;
        cout << "Please specify output file" << endl;
        cin >> input;
        FILE_NAME_LOG = input;
    }

    //However, the program will exit if the user provides only one file name. Either all file names must be set or none.
    if (FILE_NAME_INPUT == "NONE" || FILE_NAME_QUERY == "NONE" || FILE_NAME_LOG == "NONE")
    {
        cout << "Not enough files specified: " << endl;
        if (FILE_NAME_INPUT == "NONE")
            cout << "Please specify input file" << endl;
        if (FILE_NAME_QUERY == "NONE")
            cout << "Please specify query file" << endl;
        if (FILE_NAME_LOG == "NONE")
            cout << "Please specify output file" << endl;
        exit(0);
    }

    //Default values
    if (k == -1)
        k = 14; //PARAM <k>
    if (M == -1)
        M = 10; //PARAM <L>
    if (probes == -1)
        probes = 2; //PARAM <N>
    if (N == -1)
        N = 1;
    if (RANGE == -1)
        RANGE = 10000; //PARAM <radius>

    // cout << "FILE_NAME_INPUT: " << FILE_NAME_INPUT << endl;
    // cout << "FILE_NAME_QUERY: " << FILE_NAME_QUERY << endl;
    // cout << "FILE_NAME_LOG: " << FILE_NAME_LOG << endl;
    // cout << "k: " << k << endl;
    // cout << "M: " << M << endl;
    // cout << "probes: " << probes << endl;
    // cout << "N: " << N << endl;
    // cout << "RANGE: " << RANGE << endl;

    bool justOnce = true;
    int how_many_columns = 0;
    int how_many_rows = 0;
    int temp;
    string mystring;
    string tempString;

    myLogFile.open(FILE_NAME_LOG); //PARAM <output file>

    //Open dataset file to count number of rows.
    ifstream myfile;
    myfile.open(FILE_NAME_INPUT);
    how_many_rows = count(istreambuf_iterator<char>(myfile), istreambuf_iterator<char>(), '\n');
    //how_many_rows++;
    myfile.close();
    myfile.clear();

    //Now count number of columns
    myfile.open(FILE_NAME_INPUT);
    if (myfile.is_open())
    {
        while (myfile)
        {
            getline(myfile, mystring);
            stringstream sso(mystring);
            sso >> temp;
            while (justOnce && sso >> tempString)
            {
                how_many_columns++; //calculate the number of columns (dimension of the vector without the id)
            }
            justOnce = false;
        }
    }
    else
    {
        cout << "error with opening input file" << endl;
        exit(0);
    }
    myfile.close();
    myfile.clear();

    //And now initialize the VectorElement objects.
    myfile.open(FILE_NAME_INPUT);
    VectorElement **Input_Array = new VectorElement *[how_many_rows];
    int i = 0;
    if (myfile.is_open())
    {
        while (myfile)
        {
            getline(myfile, mystring);
            stringstream sso(mystring);
            if (i < how_many_rows)
            {
                Input_Array[i] = new VectorElement(how_many_columns, mystring, 0);
                i++;
            }
        }
    }
    else
    {
        cout << "error with opening input file" << endl;
        exit(0);
    }
    myfile.close();
    myfile.clear();

    // cout << "columns==== " << how_many_columns << endl;
    // cout << "rows==== " << how_many_rows << endl;
    //--------DATA COLLECTED----------

    int w = 700; //Hardcoded value for w

    //Initialize cube object and insert items
    HyperCube Cube_Obj(k, how_many_columns, w, N, M, probes, RANGE);
    for (int j = 0; j < how_many_rows; j++)
    {
        Cube_Obj.insertItem(Input_Array[j]);
    }

    //Save the query items.
    ifstream myfilequery;
    myfilequery.open(FILE_NAME_QUERY);
    int query_rows = count(istreambuf_iterator<char>(myfilequery), istreambuf_iterator<char>(), '\n');
    //how_many_rows++;
    //cout << "Query rows are: " << query_rows << endl;
    myfilequery.close();
    myfilequery.clear();
    myfilequery.open(FILE_NAME_QUERY);
    VectorElement **Query_Array = new VectorElement *[query_rows];
    int query_i = 0;
    if (myfilequery.is_open())
    {

        while (myfilequery)
        {
            getline(myfilequery, mystring);
            stringstream sso(mystring);
            if (query_i < query_rows)
            {
                Query_Array[query_i] = new VectorElement(how_many_columns, mystring, 0);
                query_i++;
            }
        }
    }
    else
    {
        cout << "error with opening query file" << endl;
        exit(0);
    }
    myfilequery.close();

    //Initialize the arrays for queries.
    Cube_Obj.initNeighboursInfo(query_rows);

    //And now for each query...
    for (int i = 0; i < query_rows; i++)
    {
        myLogFile << "Query: " << Query_Array[i]->id << endl;

        //First get the actual N neighbors with brute force check
        list<idDistancePair> PairListBF;
        std::chrono::steady_clock::time_point begin_bf = std::chrono::steady_clock::now();

        using clock = std::chrono::system_clock;
        using sec = std::chrono::duration<double>;

        const auto before_BF = clock::now();
        for (int l = 0; l < how_many_rows; l++)
        {
            Input_Array[l]->getL2Distance(Query_Array[i]);
            idDistancePair *Pair = new idDistancePair(Input_Array[l]->id, Input_Array[l]->distanceCurrQ);
            PairListBF.push_back(*Pair);
            delete Pair;
        }
        PairListBF.sort(cmpListPair);
        const sec duration_BF = clock::now() - before_BF;

        //Now find the approx N neighbors with Hypercube
        const auto before_NN = clock::now();
        Cube_Obj.getFirstProbe(Query_Array[i], i, "NN"); //The algorithm will start with getFirstProbe func and proceed.

        list<idDistancePair> PairList;
        int actual_N = Cube_Obj.Ni; //Actual N is the last value of Ni. Because search may stop from the 'M' or 'probes' arguments.
        for (int Ni = 0; Ni < actual_N; Ni++)
        { //Add a idDistancePair object for every neighbor.

            idDistancePair *Pair = new idDistancePair(Cube_Obj.neighboursInfoTable[i]->arrayId[Ni], Cube_Obj.neighboursInfoTable[i]->arrayDistance[Ni]);
            if (Pair->getDistance() == -1 || Pair->getDistance() == 0)
            {
                delete Pair;
                break;
            }

            PairList.push_back(*Pair);
            delete Pair;
        }
        PairList.sort(cmpListPair); //sort the list to get nearest values first.
        const sec duration_NN = clock::now() - before_NN;

        //coutLineWithMessage("NEAREST NEIGHBOURS ARE: ");

        //Output of NN search.
        int currNeighbours = 0;
        list<idDistancePair>::iterator hitr1 = PairList.begin();
        list<idDistancePair>::iterator hitrbf2 = PairListBF.begin();
        for (; hitr1 != PairList.end() && hitrbf2 != PairListBF.end(); ++hitr1, ++hitrbf2)
        {
            if (currNeighbours == N)
                break;

            myLogFile << "Nearest neighbor-" << (currNeighbours + 1) << ": " << hitr1->getId() << endl;
            myLogFile << "distanceHypercube: " << hitr1->getDistance() << endl;
            myLogFile << "distanceTrue: " << hitrbf2->getDistance() << endl;

            currNeighbours++;
        }

        myLogFile << "tHypercube = " << duration_NN.count() << "[s]" << endl;
        myLogFile << "tTrue = " << duration_BF.count() << "[s]" << endl;

        //Now doing Range search
        myLogFile << "R-near neighbors: " << endl;

        list<VectorElement *> cube_range_list;
        Cube_Obj.range_list = cube_range_list;
        Cube_Obj.range = RANGE;

        Cube_Obj.getFirstProbe(Query_Array[i], i, "range");
        cube_range_list = Cube_Obj.range_list;

        //Output for range search.
        list<VectorElement *>::iterator hitrR;
        for (hitrR = cube_range_list.begin(); hitrR != cube_range_list.end(); ++hitrR)
        {
            VectorElement *vobj = *hitrR;
            myLogFile << vobj->id << endl;
        }
    }

    //---DELETE MEMORY---

    for (int i = 0; i < how_many_rows; i++)
    {
        delete Input_Array[i];
    }
    delete[] Input_Array;

    //cout << "del" << endl;
    for (int i = 0; i < query_rows; i++)
    {
        delete Query_Array[i];
    }

    delete[] Query_Array;

    myLogFile.close();

    cout << "Program has successfully completed and written its results to the output file." << endl;
    return 0;
}
