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
#include <bits/stdc++.h>

#include <list>
#include <vector>

#include "Neighbours.h"
#include "VectorElement.h"
#include "CurveElement.h"
#include "LSHash.h"
#include "HyperCube.h"
#include "Helpers.h"
#include "IdDistancePair.h"
#include "DiscreteFrechet.h"

using namespace std;

int main(int argc, char *argv[])
{

    //Setting up the algorithm parameters
    string FILE_NAME_INPUT = "NONE";
    string FILE_NAME_QUERY = "NONE";
    string FILE_NAME_LOG = "NONE";
    int k_input = -1;
    int M = -1;
    int probes = -1;
    int NUMBER_OF_HASH_TABLES = -1;
    int NUMBER_OF_NEIGHBOURS = 1; //Check possible errs
    double RANGE = -1.0;
    double delta = -1.0;
    string algorithm = "LSH";
    string metric = "discrete";

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
        else if (strcmp(argv[i], "-algorithm") == 0)
        {
            
            if (strcmp(argv[i + 1], "LSH") == 0)
            {
                algorithm = "LSH";
            }
            else if (strcmp(argv[i + 1], "Hypercube") == 0)
            {
                algorithm = "Hypercube";
            }
            else if (strcmp(argv[i + 1], "Frechet") == 0)
            {
                algorithm = "Frechet";
            }
            else
            {
                cout << "Method must be Classic or LSH or Hypercube - wrong input" << endl;
                exit(0);
            }
        }
        else if (strcmp(argv[i], "-metric") == 0)
        {
            if (strcmp(argv[i + 1], "discrete") == 0)
            {
                metric = "discrete";
            }
            else if (strcmp(argv[i + 1], "continuous") == 0)
            {
                metric = "continuous";
            }
        }
        else if (strcmp(argv[i], "-delta") == 0)
        {
            string tempd;
            tempd = argv[i + 1];
            delta = args_string_to_double(tempd);
        }
        // else if (strcmp(argv[i], "-N") == 0)
        // {
        //     string tempN;
        //     tempN = argv[i + 1];
        //     NUMBER_OF_NEIGHBOURS = args_string_to_int(tempN);
        // }
        // else if (strcmp(argv[i], "-R") == 0)
        // {
        //     string tempR;
        //     tempR = argv[i + 1];
        //     RANGE = args_string_to_double(tempR);
        // }
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
    if (k_input == -1){
        if (algorithm == "LSH" || algorithm == "Frechet"){
            k_input = 4;
        }else if (algorithm == "Hypercube"){
            k_input = 14;
        }
    }
    if (NUMBER_OF_HASH_TABLES == -1)
        NUMBER_OF_HASH_TABLES = 5; //PARAM <L>
    if (M == -1)
        M = 10; //PARAM <M>
    if (probes == -1)
        probes = 2; //PARAM <probes>
    if (delta == -1.0)
        delta = 5; //PARAM <delta>
    

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
    string temp;
    string mystring;
    string tempString;

    myLogFile.open(FILE_NAME_LOG); //PARAM <output file>

    VectorElement **Input_Array;
    VectorElement **Query_Array;

    CurveElement **Input_Array_Frechet;
    CurveElement **Query_Array_Frechet;

    VectorElement **vectorized_input_storage;
    VectorElement **vectorized_query_storage;

    int query_rows = -1;
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

    cout << "columns==== " << how_many_columns << endl;
    cout << "rows==== " << how_many_rows << endl;
    //--------DATA COLLECTED----------

    //Save the query items.
    ifstream myfilequery;
    myfilequery.open(FILE_NAME_QUERY);
    query_rows = count(istreambuf_iterator<char>(myfilequery), istreambuf_iterator<char>(), '\n');
    //how_many_rows++;
    cout << "Query rows are: " << query_rows << endl;
    myfilequery.close();
    myfilequery.clear();
        
    if (algorithm == "LSH" || algorithm == "Hypercube"){
        
        //And now initialize the VectorElement objects.
        myfile.open(FILE_NAME_INPUT);
        Input_Array = new VectorElement *[how_many_rows];
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

        myfilequery.open(FILE_NAME_QUERY);
        Query_Array = new VectorElement *[query_rows];
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
        
        // for (int i = 0; i < how_many_rows; i++)
        // {
        //     cout << Input_Array[i]->id << endl;
        //     myLogFile<<"END"<<endl;
        // }

        // for (int i = 0; i < query_rows; i++)
        // {
        //     cout << Query_Array[i]->id << endl;
        //     myLogFile<<"END"<<endl;
        // }

    }else if (algorithm == "Frechet"){
        
        //And now initialize the VectorElement objects.
        myfile.open(FILE_NAME_INPUT);
        Input_Array_Frechet = new CurveElement *[how_many_rows];
        int i = 0;
        if (myfile.is_open())
        {
            while (myfile)
            {
                getline(myfile, mystring);
                stringstream sso(mystring);
                if (i < how_many_rows)
                {
                    if (metric == "discrete"){
                        Input_Array_Frechet[i] = new CurveElement(how_many_columns, mystring, NUMBER_OF_HASH_TABLES,"discrete");
                    }else{
                        Input_Array_Frechet[i] = new CurveElement(how_many_columns, mystring, NUMBER_OF_HASH_TABLES,"continuous");
                    }
                    
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


        myfilequery.open(FILE_NAME_QUERY);
        Query_Array_Frechet = new CurveElement *[query_rows];
        int query_i = 0;
        if (myfilequery.is_open())
        {         
            while (myfilequery)
            {
                getline(myfilequery, mystring);
                stringstream sso(mystring);
                if (query_i < query_rows)
                {
                    if (metric == "discrete"){
                        Query_Array_Frechet[query_i] = new CurveElement(how_many_columns, mystring, NUMBER_OF_HASH_TABLES,"discrete");
                    }else{
                        Query_Array_Frechet[query_i] = new CurveElement(how_many_columns, mystring, NUMBER_OF_HASH_TABLES,"continuous");
                    }
                    
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

        // for (int i = 0; i < how_many_rows; i++)
        // {
        //     Input_Array_Frechet[i]->displayVectorElementArray();
        //     myLogFile<<"END"<<endl;
        // }

        // myLogFile << "NOW QUERY" << endl;
        // for (int i = 0; i < query_rows; i++)
        // {
        //     Query_Array_Frechet[i]->displayVectorElementArray();
        //     myLogFile<<"END"<<endl;
        // }


    }

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

    LSHash **Hash_Array;

    if (algorithm == "LSH"){
        
        int NUMBER_OF_BUCKETS = how_many_rows / 8;

        //Creatign array of Hash tables.
        Hash_Array = new LSHash *[NUMBER_OF_HASH_TABLES];
        for (int i = 0; i < NUMBER_OF_HASH_TABLES; i++)
        {
            Hash_Array[i] = new LSHash(NUMBER_OF_BUCKETS, how_many_columns, k_input, w_arg);
            
            for (int j = 0; j < how_many_rows; j++)
            {
                Hash_Array[i]->insertItem(Input_Array[j], r_array);
            }

            Hash_Array[i]->initNeighboursInfo(query_rows, NUMBER_OF_NEIGHBOURS);
        }

   
        //And now for each query...
        double tTrueSum = 0.0;
        double tApproxSum = 0.0;
        double last_maf = -1.0;
        list<idDistancePair> PairList;
        for (int i = 0; i < query_rows; i++)
        {
            myLogFile << "Query: " << Query_Array[i]->id << endl;
            myLogFile << "Algorithm: LSH_Vector " << endl;

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

                Hash_Array[j]->calculateDistanceAndFindN(Query_Array[i], r_array, i, NUMBER_OF_NEIGHBOURS, "LSH");

                for (int k = 0; k < NUMBER_OF_NEIGHBOURS; k++)
                { //Add a idDistancePair object for every neighbor.
                    idDistancePair *Pair = new idDistancePair(Hash_Array[j]->neighboursInfoTable[i]->arrayId[k], Hash_Array[j]->neighboursInfoTable[i]->arrayDistance[k]);
                    if (Pair->getDistance() <= -1 )//|| Pair->getDistance() == 0)
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

                myLogFile << "Approximate Nearest neighbor-" << (currNeighbours + 1) << ": " << hitr1->getId() << endl;
                myLogFile << "True Nearest neighbor-" << (currNeighbours + 1) << ": " << hitrbf2->getId() << endl;
                myLogFile << "distanceApproximate: " << hitr1->getDistance() << endl;
                myLogFile << "distanceTrue: " << hitrbf2->getDistance() << endl;

                double curr_maf = hitr1->getDistance() / hitrbf2->getDistance();
                if (curr_maf > last_maf){
                    last_maf = curr_maf;
                } 

                currNeighbours++;
            }

            //myLogFile << "tLSH = " << duration_NN.count() << "[s]" << endl;
            //myLogFile << "tTrue = " << duration_BF.count() << "[s]" << endl;
            tApproxSum += duration_NN.count();
            tTrueSum += duration_BF.count();
            
            //reset the list for new q
            PairList.clear();
            
        }

        myLogFile << "tApproximateAverage = " << tApproxSum/query_rows << "[s]" << endl;
        myLogFile << "tTrueAverage = " << tTrueSum/query_rows << "[s]" << endl;
        myLogFile << "MAF = " << last_maf << endl;


    }else if (algorithm == "Hypercube"){
    
        //Initialize cube object and insert items
        HyperCube Cube_Obj(k_input, how_many_columns, w_arg, NUMBER_OF_NEIGHBOURS, M, probes, RANGE);
        for (int j = 0; j < how_many_rows; j++)
        {
            Cube_Obj.insertItem(Input_Array[j]);
        }

        //Initialize the arrays for queries.
        Cube_Obj.initNeighboursInfo(query_rows);

        double tTrueSum = 0.0;
        double tApproxSum = 0.0;
        double last_maf = -1.0;
        //And now for each query...
        for (int i = 0; i < query_rows; i++)
        {
            myLogFile << "Query: " << Query_Array[i]->id << endl;
            myLogFile << "Algorithm: Hypercube " << endl;

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
                if (currNeighbours == NUMBER_OF_NEIGHBOURS)
                    break;

                myLogFile << "Approximate Nearest neighbor-" << (currNeighbours + 1) << ": " << hitr1->getId() << endl;
                myLogFile << "True Nearest neighbor-" << (currNeighbours + 1) << ": " << hitrbf2->getId() << endl;
                myLogFile << "distanceApproximate: " << hitr1->getDistance() << endl;
                myLogFile << "distanceTrue: " << hitrbf2->getDistance() << endl;

                double curr_maf = hitr1->getDistance() / hitrbf2->getDistance();
                if (curr_maf > last_maf){
                    last_maf = curr_maf;
                } 

                currNeighbours++;
            }

            //myLogFile << "tHypercube = " << duration_NN.count() << "[s]" << endl;
            //myLogFile << "tTrue = " << duration_BF.count() << "[s]" << endl;
            tApproxSum += duration_NN.count();
            tTrueSum += duration_BF.count();

        }

        myLogFile << "tApproximateAverage = " << tApproxSum/query_rows << "[s]" << endl;
        myLogFile << "tTrueAverage = " << tTrueSum/query_rows << "[s]" << endl;
        myLogFile << "MAF = " << last_maf << endl;

    
    }else if (algorithm == "Frechet" && metric == "discrete"){
 
        int NUMBER_OF_BUCKETS = how_many_rows / 8;

        uniform_int_distribution<> UM(100000, INT_MAX-1000000);
        int M = UM(e);
        cout << M << endl;

        vectorized_input_storage = new VectorElement * [how_many_rows];
       
        Hash_Array = new LSHash *[NUMBER_OF_HASH_TABLES];
        for (int l = 0; l < NUMBER_OF_HASH_TABLES; l++){

            Hash_Array[l] = new LSHash(NUMBER_OF_BUCKETS, how_many_columns*2, k_input, w_arg);

            unsigned seed = chrono::steady_clock::now().time_since_epoch().count();
            default_random_engine e(seed);
            
            double open_d = delta - 1;
            uniform_real_distribution<> U(0.0, open_d);
            double t1 = U(e);
            double t2 = U(e);
            Hash_Array[l]->t1 = t1;
            Hash_Array[l]->t2 = t2;

            for (int i = 0; i < how_many_rows; i++)
            {

                Input_Array_Frechet[i]->Snapping2d(t1,t2,delta,how_many_columns);
                string str_curve = Input_Array_Frechet[i]->Vectorization2d(how_many_columns,M);

                VectorElement* vec2add = new VectorElement(how_many_columns*2, str_curve, NUMBER_OF_HASH_TABLES);
                vec2add->original_curve = Input_Array_Frechet[i];
                
                Hash_Array[l]->insertItem(vec2add, r_array);
                
                //TODO: ALSO refactor
                vectorized_input_storage[i] = vec2add;
                Input_Array_Frechet[i]->gridElementTwoD.clear();    
                //myLogFile << "END OF ARR" << endl;
            }
        
            //myLogFile << "END OF L" << endl;
            Hash_Array[l]->initNeighboursInfo(query_rows, NUMBER_OF_NEIGHBOURS);
        }

        //query stuff
        vectorized_query_storage = new VectorElement *[query_rows];

        double tTrueSum = 0.0;
        double tApproxSum = 0.0;
        double last_maf = -1.0;
        list<idDistancePair> PairList;
        for (int i = 0; i < query_rows; i++)
        {
            myLogFile << "Query: " << Query_Array_Frechet[i]->id << endl;
            myLogFile << "Algorithm: LSH_Frechet_Discrete " << endl;

            //First get the actual N neighbors with brute force check
            std::chrono::steady_clock::time_point begin_bf = std::chrono::steady_clock::now();

            using clock = std::chrono::system_clock;
            using sec = std::chrono::duration<double>;

            const auto before_BF = clock::now();
            list<idDistancePair> PairListBF;
            for (int l = 0; l < how_many_rows; l++)
            {

                //double dfd = 0.0; //skipping true calculations
                double dfd = ret_DFD(how_many_columns,how_many_columns, Input_Array_Frechet[l]->arrayElementTwoD,Query_Array_Frechet[i]->arrayElementTwoD);

                idDistancePair *Pair = new idDistancePair(Input_Array_Frechet[l]->id, dfd);
                PairListBF.push_back(*Pair);
                delete Pair;
            }
            PairListBF.sort(cmpListPair);
            const sec duration_BF = clock::now() - before_BF;


            //Now find the approx N neighbors with LSH
            const auto before_NN = clock::now();
            for (int j = 0; j < NUMBER_OF_HASH_TABLES; j++)
            {

                double t1 = Hash_Array[j]->t1;
                double t2 = Hash_Array[j]->t2;
                Query_Array_Frechet[i]->Snapping2d(t1,t2,delta,how_many_columns);
                string str_curve = Query_Array_Frechet[i]->Vectorization2d(how_many_columns,M);
                
                VectorElement* vec2add = new VectorElement(how_many_columns*2, str_curve, NUMBER_OF_HASH_TABLES);
                vec2add->original_curve = Query_Array_Frechet[i];

                Hash_Array[j]->calculateDistanceAndFindN(vec2add, r_array, i, NUMBER_OF_NEIGHBOURS, "Frechet");

                vectorized_query_storage[i] = vec2add;
                
                for (int k = 0; k < NUMBER_OF_NEIGHBOURS; k++)
                { //Add a idDistancePair object for every neighbor.

                    //cout << "id " << Hash_Array[j]->neighboursInfoTable[i]->arrayId[k] << " dis " << Hash_Array[j]->neighboursInfoTable[i]->arrayDistance[k] << endl;
                    idDistancePair *Pair = new idDistancePair(Hash_Array[j]->neighboursInfoTable[i]->arrayId[k], Hash_Array[j]->neighboursInfoTable[i]->arrayDistance[k]);
                    if (Pair->getDistance() <= -1 )//|| Pair->getDistance() == 0 || Pair->getId() == " ")
                    {
                    //    cout<< "? "<< endl;
                        delete Pair;
                        break;
                    }
                    PairList.push_back(*Pair);
                    delete Pair;
                }

                //delete vec2add;
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

                myLogFile << "Approximate Nearest neighbor-" << (currNeighbours + 1) << ": " << hitr1->getId() << endl;
                myLogFile << "True Nearest neighbor-" << (currNeighbours + 1) << ": " << hitrbf2->getId() << endl;
                myLogFile << "distanceApproximate: " << hitr1->getDistance() << endl;
                myLogFile << "distanceTrue: " << hitrbf2->getDistance() << endl;

                double curr_maf = hitr1->getDistance() / hitrbf2->getDistance();
                if (curr_maf > last_maf){
                    last_maf = curr_maf;
                } 

                currNeighbours++;
            }

            //myLogFile << "tLSH = " << duration_NN.count() << "[s]" << endl;
            //myLogFile << "tTrue = " << duration_BF.count() << "[s]" << endl;
            tApproxSum += duration_NN.count();
            tTrueSum += duration_BF.count();

            //reset the list for new q
            PairList.clear();

            //cout << "query end" << endl;            
        }

        myLogFile << "tApproximateAverage = " << tApproxSum/query_rows << "[s]" << endl;
        myLogFile << "tTrueAverage = " << tTrueSum/query_rows << "[s]" << endl;
        myLogFile << "MAF = " << last_maf << endl;//TODO

    
    }else{

        int NUMBER_OF_BUCKETS = how_many_rows / 8;
        LSHash Hash_Obj(NUMBER_OF_BUCKETS, how_many_columns, k_input, w_arg);
        Hash_Obj.initNeighboursInfo(query_rows, NUMBER_OF_NEIGHBOURS);

        uniform_int_distribution<> UM(100000, INT_MAX-1000000);
        int M = UM(e);
        cout << M << endl;

        double open_d = delta - 1; 
        uniform_real_distribution<> U(0.0, open_d);
        double t1 = U(e);
        //Hash_Obj.t1 = t1;

        vectorized_input_storage = new VectorElement * [how_many_rows];
        
        for (int i = 0; i < how_many_rows; i++)
        {

            double e = 3;
            Input_Array_Frechet[i]->Filtering(e);
            cout<<"size" << Input_Array_Frechet[i]->filteredElementOneD.size() << endl;
            Input_Array_Frechet[i]->Snapping1d(t1,delta);
            Input_Array_Frechet[i]->MinMax();
            string str_curve = Input_Array_Frechet[i]->Vectorization1d(how_many_columns, M);
            
            VectorElement* vec2add = new VectorElement(how_many_columns, str_curve, NUMBER_OF_HASH_TABLES);
            vec2add->original_curve = Input_Array_Frechet[i];
            
            Hash_Obj.insertItem(vec2add, r_array); //do some checks

            vectorized_input_storage[i] = vec2add;
            //myLogFile << "END OF ARR" << endl;
        }

        vectorized_query_storage = new VectorElement * [query_rows];

        double tTrueSum = 0.0;
        double tApproxSum = 0.0;
        double last_maf = -1.0;
        //And now for each query...
        list<idDistancePair> PairList;
        for (int i = 0; i < query_rows; i++)
        {
            myLogFile << "Query: " << Query_Array_Frechet[i]->id << endl;
            myLogFile << "Algorithm: LSH_Frechet_Continuous " << endl;

            double e = 3;
            Query_Array_Frechet[i]->Filtering(e);
            cout<<"size" << Query_Array_Frechet[i]->filteredElementOneD.size() << endl;

            //First get the actual N neighbors with brute force check
            list<idDistancePair> PairListBF;
            std::chrono::steady_clock::time_point begin_bf = std::chrono::steady_clock::now();

            using clock = std::chrono::system_clock;
            using sec = std::chrono::duration<double>;

            const auto before_BF = clock::now();
            for (int l = 0; l < how_many_rows; l++)
            {
                //Input_Array[l]->getL2Distance(Query_Array[i]);
                double cfd = ret_CFD(Input_Array_Frechet[l],Query_Array_Frechet[i]);

                idDistancePair *Pair = new idDistancePair(Input_Array_Frechet[l]->id, cfd);
                PairListBF.push_back(*Pair);
                delete Pair;
            }
            PairListBF.sort(cmpListPair);
            const sec duration_BF = clock::now() - before_BF;

            //Now find the approx N neighbors with Hypercube
            const auto before_NN = clock::now();

            //double t1 = Hash_Obj.t1;
            Query_Array_Frechet[i]->Snapping1d(t1,delta);
            Query_Array_Frechet[i]->MinMax();
            string str_curve = Query_Array_Frechet[i]->Vectorization1d(how_many_columns, M);
            
            VectorElement* vec2add = new VectorElement(how_many_columns, str_curve, NUMBER_OF_HASH_TABLES);
            vec2add->original_curve = Query_Array_Frechet[i];
    
            Hash_Obj.calculateDistanceAndFindN(vec2add, r_array, i, NUMBER_OF_NEIGHBOURS, "Frechet_Cont");

            vectorized_query_storage[i] = vec2add;
                
            for (int k = 0; k < NUMBER_OF_NEIGHBOURS; k++)
            { //Add a idDistancePair object for every neighbor.

                idDistancePair *Pair = new idDistancePair(Hash_Obj.neighboursInfoTable[i]->arrayId[k], Hash_Obj.neighboursInfoTable[i]->arrayDistance[k]);
                if (Pair->getDistance() <= -1 )//|| Pair->getDistance() == 0 || Pair->getId() == " ")
                {
                    delete Pair;
                    break;
                }
                PairList.push_back(*Pair);
                delete Pair;
            }
            PairList.sort(cmpListPair);                                //sort the list to get nearest values first.
            const sec duration_NN = clock::now() - before_NN;

            //coutLineWithMessage("NEAREST NEIGHBOURS ARE: ");

            //Output of NN search.
            int currNeighbours = 0;
            list<idDistancePair>::iterator hitr1 = PairList.begin();
            list<idDistancePair>::iterator hitrbf2 = PairListBF.begin();
            for (; hitr1 != PairList.end() && hitrbf2 != PairListBF.end(); ++hitr1, ++hitrbf2)
            {
                if (currNeighbours == NUMBER_OF_NEIGHBOURS)
                    break;

                myLogFile << "Approximate Nearest neighbor-" << (currNeighbours + 1) << ": " << hitr1->getId() << endl;
                myLogFile << "True Nearest neighbor-" << (currNeighbours + 1) << ": " << hitrbf2->getId() << endl;
                myLogFile << "distanceApproximate: " << hitr1->getDistance() << endl;
                myLogFile << "distanceTrue: " << hitrbf2->getDistance() << endl;

                double curr_maf = hitr1->getDistance() / hitrbf2->getDistance();
                if (curr_maf > last_maf){
                    last_maf = curr_maf;
                } 

                currNeighbours++;
            }

            tApproxSum += duration_NN.count();
            tTrueSum += duration_BF.count();

        }

        myLogFile << "tApproximateAverage = " << tApproxSum/query_rows << "[s]" << endl;
        myLogFile << "tTrueAverage = " << tTrueSum/query_rows << "[s]" << endl;
        myLogFile << "MAF = " << last_maf << endl;

    }

    //---DELETE MEMORY---

    delete[] r_array;

    if (algorithm == "LSH" || (algorithm == "Frechet" && metric == "discrete") ){
        
        for (int i = 0; i < NUMBER_OF_HASH_TABLES; i++)
        {
            delete Hash_Array[i];
        }
        delete[] Hash_Array;
    }

    if (algorithm == "LSH" || algorithm == "Hypercube"){

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

    }else if (algorithm == "Frechet"){

        for (int i = 0; i < how_many_rows; i++)
        {
            delete Input_Array_Frechet[i];
            delete vectorized_input_storage[i];
        }
        delete[] Input_Array_Frechet;
        delete[] vectorized_input_storage;

        for (int i = 0; i < query_rows; i++)
        {
            delete Query_Array_Frechet[i];
            delete vectorized_query_storage[i];
        }

        delete[] Query_Array_Frechet;
        delete[] vectorized_query_storage;

    }

    myLogFile.close();
    cout << "Program has successfully completed and written its results to the output file." << endl;

    
    return 0;
}
