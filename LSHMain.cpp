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
#include <cstring>

#include <list>
#include <vector>

#include "Neighbours.h"
#include "VectorElement.h"
#include "LSHash.h"
#include "Helpers.h"
#include "IdDistancePair.h"

using namespace std;

int main(int argc, char *argv[])
{

    //Setting up the algorithm parameters
    string FILE_NAME_INPUT = "NONE";
    string FILE_NAME_QUERY = "NONE";
    string FILE_NAME_LOG = "NONE";
    int k_input = -1;
    int NUMBER_OF_HASH_TABLES = -1;
    int NUMBER_OF_NEIGHBOURS = -1;
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
            string temp;
            temp = argv[i + 1];
            k_input = args_string_to_int(temp);
        }
        else if (strcmp(argv[i], "-L") == 0)
        {
            string temp;
            temp = argv[i + 1];
            NUMBER_OF_HASH_TABLES = args_string_to_int(temp);
        }
        else if (strcmp(argv[i], "-o") == 0)
        {
            FILE_NAME_LOG = argv[i + 1];
        }
        else if (strcmp(argv[i], "-N") == 0)
        {
            string tempN;
            tempN = argv[i + 1];
            NUMBER_OF_NEIGHBOURS = args_string_to_int(tempN);
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

    //Setting default parameters.
    if (k_input == -1)
        k_input = 4; //PARAM <k>
    if (NUMBER_OF_HASH_TABLES == -1)
        NUMBER_OF_HASH_TABLES = 5; //PARAM <L>
    if (NUMBER_OF_NEIGHBOURS == -1)
        NUMBER_OF_NEIGHBOURS = 1; //PARAM <N>
    if (RANGE == -1)
        RANGE = 10000; //PARAM <radius>

    // cout << "FILE_NAME_INPUT: " << FILE_NAME_INPUT << endl;
    // cout << "FILE_NAME_QUERY: " << FILE_NAME_QUERY << endl;
    // cout << "FILE_NAME_LOG: " << FILE_NAME_LOG << endl;
    // cout << "k_input: " << k_input << endl;
    // cout << "NUMBER OF HASH TABLES: " << NUMBER_OF_HASH_TABLES << endl;
    // cout << "NUMBER OF NEIGHBOURS: " << NUMBER_OF_NEIGHBOURS << endl;
    // cout << "RANGE: " << RANGE << endl;

    int w_arg = 700; //Hardcoded value for w
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
    //coutLineWithMessage(how_many_rows, "dataset rows are:");
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
                Input_Array[i] = new VectorElement(how_many_columns, mystring, NUMBER_OF_HASH_TABLES);
                i++;
            }
        }
    }
    else
    {
        cout << "error with REopening input file" << endl;
        exit(0);
    }
    myfile.close();
    myfile.clear();

    myLogFile << "columns==== " << how_many_columns << endl;
    myLogFile << "rows==== " << how_many_rows << endl;
    //--------DATA COLLECTED----------

    int NUMBER_OF_BUCKETS = how_many_rows / 8;

    //R array is here. Sets r values using uniform dist up to max int
    unsigned seed = chrono::steady_clock::now().time_since_epoch().count();
    default_random_engine e(seed);

    uniform_int_distribution<> Ur(0, INT_MAX);
    int *r_array = new int[k_input];

    for (int i = 0; i < k_input; i++)
    {
        int r_val = Ur(e);
        r_array[i] = r_val;
    }

    //Creatign array of Hash tables.
    LSHash **Hash_Array = new LSHash *[NUMBER_OF_HASH_TABLES];
    for (int i = 0; i < NUMBER_OF_HASH_TABLES; i++)
    {
        Hash_Array[i] = new LSHash(NUMBER_OF_BUCKETS, how_many_columns, k_input, w_arg);
    }

    //Inserting the items
    for (int i = 0; i < NUMBER_OF_HASH_TABLES; i++)
    {
        for (int j = 0; j < how_many_rows; j++)
        {
            Hash_Array[i]->insertItem(Input_Array[j], r_array);
        }
    }

    //Save the query items.
    ifstream myfilequery;
    myfilequery.open(FILE_NAME_QUERY);
    int query_rows = count(istreambuf_iterator<char>(myfilequery), istreambuf_iterator<char>(), '\n');
    //how_many_rows++;
    cout << "Query rows are: " << query_rows << endl;
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
                Query_Array[query_i] = new VectorElement(how_many_columns, mystring, NUMBER_OF_HASH_TABLES);
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
    for (int i = 0; i < NUMBER_OF_HASH_TABLES; i++) //for each hash table
    {
        Hash_Array[i]->initNeighboursInfo(query_rows, NUMBER_OF_NEIGHBOURS);
    }

    //And now for each query...
    list<idDistancePair> PairList;
    for (int i = 0; i < query_rows; i++)
    {
        myLogFile << "Query: " << Query_Array[i]->id << endl;

        //First get the actual N neighbors with brute force check
        std::chrono::steady_clock::time_point begin_bf = std::chrono::steady_clock::now();

        using clock = std::chrono::system_clock;
        using sec = std::chrono::duration<double>;

        const auto before_BF = clock::now();
        list<idDistancePair> PairListBF;
        for (int l = 0; l < how_many_rows; l++)
        {
            Input_Array[l]->getL2Distance(Query_Array[i]);
            idDistancePair *Pair = new idDistancePair(Input_Array[l]->id, Input_Array[l]->distanceCurrQ);
            PairListBF.push_back(*Pair);
            delete Pair;
        }
        PairListBF.sort(cmpListPair);
        const sec duration_BF = clock::now() - before_BF;

        //Now find the approx N neighbors with LSH
        const auto before_NN = clock::now();
        for (int j = 0; j < NUMBER_OF_HASH_TABLES; j++)
        {

            Hash_Array[j]->calculateDistanceAndFindN(Query_Array[i], r_array, i, NUMBER_OF_NEIGHBOURS);

            for (int k = 0; k < NUMBER_OF_NEIGHBOURS; k++)
            { //Add a idDistancePair object for every neighbor.
                idDistancePair *Pair = new idDistancePair(Hash_Array[j]->neighboursInfoTable[i]->arrayId[k], Hash_Array[j]->neighboursInfoTable[i]->arrayDistance[k]);
                if (Pair->getDistance() == -1 || Pair->getDistance() == 0)
                {
                    delete Pair;
                    break;
                }
                PairList.push_back(*Pair);
                delete Pair;
            }
        }
        PairList.sort(cmpListPair);                                //sort the list to get nearest values first.
        auto last = std::unique(PairList.begin(), PairList.end()); //and remove duplicate values because we may get them from the various hash tables.
        PairList.erase(last, PairList.end());
        const sec duration_NN = clock::now() - before_NN;

        //Output of NN search.
        int currNeighbours = 0;
        list<idDistancePair>::iterator hitr1 = PairList.begin();
        list<idDistancePair>::iterator hitrbf2 = PairListBF.begin();
        for (; hitr1 != PairList.end() && hitrbf2 != PairListBF.end(); ++hitr1, ++hitrbf2)
        {
            if (currNeighbours == NUMBER_OF_NEIGHBOURS)
                break;

            myLogFile << "Nearest neighbor-" << (currNeighbours + 1) << ": " << hitr1->getId() << endl;
            myLogFile << "distanceLSH: " << hitr1->getDistance() << endl;
            myLogFile << "distanceTrue: " << hitrbf2->getDistance() << endl;
            currNeighbours++;
        }

        myLogFile << "tLSH = " << duration_NN.count() << "[s]" << endl;
        myLogFile << "tTrue = " << duration_BF.count() << "[s]" << endl;

        //reset the list for new q
        PairList.clear();

        //Now doing Range search
        myLogFile << "R-near neighbors: " << endl;
        for (int j = 0; j < NUMBER_OF_HASH_TABLES; j++)
        {
            list<VectorElement *> lsh_range_list;
            Hash_Array[j]->range_list = lsh_range_list;

            Hash_Array[j]->RangeSearch(Query_Array[i], r_array, i, RANGE);
            lsh_range_list = Hash_Array[j]->range_list;

            //Output for range search.
            list<VectorElement *>::iterator hitr1;
            for (hitr1 = lsh_range_list.begin(); hitr1 != lsh_range_list.end(); ++hitr1)
            {
                VectorElement *vobj = *hitr1;
                myLogFile << vobj->id << endl;
            }
        }
    }

    //---DELETE MEMORY---

    for (int i = 0; i < NUMBER_OF_HASH_TABLES; i++)
    {
        delete Hash_Array[i];
    }
    delete[] Hash_Array;

    delete[] r_array;

    for (int i = 0; i < how_many_rows; i++)
    {
        delete Input_Array[i];
    }
    delete[] Input_Array;

    for (int i = 0; i < query_rows; i++)
    {
        delete Query_Array[i];
    }

    delete[] Query_Array;

    myLogFile.close();

    cout << "Program has successfully completed and written its results to the output file." << endl;
    return 0;
}
