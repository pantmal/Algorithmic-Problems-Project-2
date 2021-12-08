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
//#include "HyperCube.h"
#include "VectorElement.h"
#include "Helpers.h"
#include "Cluster.h"
#include "KMeans.h"


using namespace std;

int main(int argc, char *argv[])
{
    
    //Setting up the algorithm parameters
    string FILE_NAME_INPUT = "NONE";
    string FILE_CONFIG = "NONE";
    string FILE_NAME_LOG = "NONE";
    int clusters = -1;              //number_of_clusters
    int kdim = -1;                  //number_of_hypercube dimensions
    int k_input = -1;               //number_of_vecor_hash_functions
    int M = -1;                     //max_number_M_of_hypercube
    int probes = -1;                //number of probes
    int NUMBER_OF_HASH_TABLES = -1; //number_of_vector_hash_tables
    bool complete = false;
    string assigner = "Classic"; //method. Will use Classic for assignment if none provided.
    
    for (int i = 1; i < argc; i++){

        if (strcmp(argv[i], "-i") == 0)
        {
            FILE_NAME_INPUT = argv[i + 1];
        }
        else if (strcmp(argv[i], "-c") == 0)
        {
            FILE_CONFIG = argv[i + 1];
        }
        else if (strcmp(argv[i], "-o") == 0)
        {
            FILE_NAME_LOG = argv[i + 1];
        }
        else if (strcmp(argv[i], "-complete") == 0)
        {
            if (strcmp(argv[i + 1], "true") == 0)
            {
                complete = true;
            }
            else if (strcmp(argv[i + 1], "false") == 0)
            {
                complete = false;
            }
        }
        else if (strcmp(argv[i], "-m") == 0)
        {
            if (strcmp(argv[i + 1], "Classic") == 0)
            {
                assigner = "Classic";
            }
            else if (strcmp(argv[i + 1], "LSH") == 0)
            {
                assigner = "LSH";
            }
            else if (strcmp(argv[i + 1], "Hypercube") == 0)
            {
                assigner = "Hypercube";
            }
            else
            {
                cout << "Method must be Classic or LSH or Hypercube - wrong input" << endl;
                exit(0);
            }
        }
    }

    //If no file is provided the program will provide command prompts for the user to enter.
    if (FILE_NAME_INPUT == "NONE" && FILE_CONFIG == "NONE" && FILE_NAME_LOG == "NONE")
    {
        string input;
        cout << "Please specify input file" << endl;
        cin >> input;
        FILE_NAME_INPUT = input;
        cout << "Please specify configuration file" << endl;
        cin >> input;
        FILE_CONFIG = input;
        cout << "Please specify output file" << endl;
        cin >> input;
        FILE_NAME_LOG = input;
    }

    //However, the program will exit if the user provides only one file name. Either all file names must be set or none.
    if (FILE_NAME_INPUT == "NONE" || FILE_CONFIG == "NONE" || FILE_NAME_LOG == "NONE")
    {
        cout << "you need to specify all files to execute this programme" << endl;
        if (FILE_NAME_INPUT == "NONE")
            cout << "Please specify input file" << endl;
        exit(0);
        if (FILE_CONFIG == "NONE")
            cout << "Please specify configuration file" << endl;
        exit(0);
        if (FILE_NAME_LOG == "NONE")
            cout << "Please specify output file" << endl;
        exit(0);
    }

    string line;
    ifstream myConfigFile;
    myConfigFile.open(FILE_CONFIG);

    if (!myConfigFile.is_open())
    {
        perror("Error open");
        exit(EXIT_FAILURE);
    }
    //coutLineWithMessage("checking the config file");
    
    //Parsing the config file and getting its arguments
    string delimiter = ":";
    while (getline(myConfigFile, line))
    {
        string argument = line.substr(0, line.find(delimiter));
        string sValue = line.substr(line.find(delimiter) + 1, line.length());
        int value = string_into_int(sValue);
        if (argument.compare("number_of_clusters") == 0)
        {
            clusters = value;
        }
        else if (argument.compare("number_of_vector_hash_tables") == 0)
        {
            NUMBER_OF_HASH_TABLES = value;
        }
        else if (argument.compare("number_of_vector_hash_functions") == 0)
        {
            k_input = value;
        }
        else if (argument.compare("max_number_M_hypercube") == 0)
        {
            M = value;
        }
        else if (argument.compare("number_of_hypercube_dimensions") == 0)
        {
            kdim = value;
        }
        else if (argument.compare("number_of_probes") == 0)
        {
            probes = value;
        }
    }

    //Cluster number can't be empty
    if (clusters == -1){
        cout<<"You need to specify clusters value. Programme will exit"<<endl;
        exit(0);
    }
    if (NUMBER_OF_HASH_TABLES == -1)
        NUMBER_OF_HASH_TABLES = 3; //PARAM <L>
    if (k_input == -1)
        k_input = 4; //PARAM <N>
    if (M == -1)
        M =10 ; //PARAM <radius>
    if (kdim == -1)
        kdim = 3; //PARAM 
    if (probes == -1)
        probes = 2; //PARAM 

    // cout << "number_of_clusters: " << clusters << endl;
    // cout << "number_of_vector_hash_tables: " << NUMBER_OF_HASH_TABLES << endl;
    // cout << "number_of_vector_hash_functions: " << k_input << endl;
    // cout << "max_number_M_hypercube: " << M << endl;
    // cout << "number_of_hypercube_dimensions: " << kdim << endl;
    // cout << "number_of_probes: " << probes << endl;

    
    bool justOnce = true;
    int how_many_columns = 0;
    int how_many_rows = 0;
    int temp;
    string mystring;
    string tempString;

    myLogFile.open(FILE_NAME_LOG);

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
    myfile.close();
    myfile.clear();

    //cout << "columns==== " << how_many_columns << endl;
    //cout << "rows==== " << how_many_rows << endl;
    //--------DATA COLLECTED----------
    
    int NUMBER_OF_BUCKETS = how_many_rows / 8;

    int N = 0; //N is unused. It is set to 0 because it is a HyperCube constructor argument.
    int w = 100; //Hardcoded value for w.

    //KMeans object is constructed and its centroid are initialized.
    KMeans kmeans_obj(assigner, clusters);
    kmeans_obj.initialization(Input_Array, how_many_rows);

    //Setting up the HyperCube and LSHash objects if need be.
    int *r_array;
    if (kmeans_obj.assigner == "Hypercube")
    {
        kmeans_obj.KMeans_Hyper = new HyperCube(kdim, how_many_columns, w, N, M, probes, 0.0);
        kmeans_obj.KMeans_Hyper->cluster_mode = true;

        for (int j = 0; j < how_many_rows; j++)
        {
            kmeans_obj.KMeans_Hyper->insertItem(Input_Array[j]);
        }
    }
    else if (kmeans_obj.assigner == "LSH")
    {
        kmeans_obj.KMeans_Hash_Array = new LSHash *[NUMBER_OF_HASH_TABLES];
        kmeans_obj.hashes = NUMBER_OF_HASH_TABLES;

        for (int i = 0; i < NUMBER_OF_HASH_TABLES; i++)
        {
            kmeans_obj.KMeans_Hash_Array[i] = new LSHash(NUMBER_OF_BUCKETS, how_many_columns, k_input, w);
            kmeans_obj.KMeans_Hash_Array[i]->cluster_mode = true;
        }

        unsigned seed = chrono::steady_clock::now().time_since_epoch().count();
        default_random_engine e(seed);
        uniform_int_distribution<> Ur(0, INT_MAX);
        r_array = new int[k_input];

        for (int i = 0; i < k_input; i++)
        {
            int r_val = Ur(e);
            r_array[i] = r_val;
        }

        kmeans_obj.r_array = r_array;
        for (int i = 0; i < NUMBER_OF_HASH_TABLES; i++)
        {
            for (int j = 0; j < how_many_rows; j++)
            {
                kmeans_obj.KMeans_Hash_Array[i]->insertItem(Input_Array[j], r_array);
            }
        }
    }

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    using clock = std::chrono::system_clock;
    using sec = std::chrono::duration<double>;

    const auto before = clock::now();

    //Hardcoded value for clustering iterations.
    int iterations = 7;
    for (int i = 0; i < iterations; i++){ 
        
        //Assignment step
        if (kmeans_obj.assigner == "Classic"){
            kmeans_obj.ClassicAssignment(Input_Array, how_many_rows);
        }
        else if (kmeans_obj.assigner == "Hypercube" || kmeans_obj.assigner == "LSH"){
            kmeans_obj.ReverseAssignment(Input_Array, how_many_rows);
        }

        //Update step
        kmeans_obj.update(how_many_columns);

        //Clearing up some valus for the next pass.
        for (int j = 0; j < how_many_rows; j++){
            Input_Array[j]->assigned = false;
            Input_Array[j]->assigned_clusters.clear();
        }
        if (i != (iterations-1)){
            
            for (int k1 = 0; k1 < clusters; k1++){
                kmeans_obj.ClusterArray[k1]->cluster_elements.clear();
            }

            if (kmeans_obj.assigner == "Hypercube"){ 
                kmeans_obj.KMeans_Hyper->assigned_total = 0;
            }
            else if (kmeans_obj.assigner == "LSH"){
                kmeans_obj.KMeans_Hash_Array[0]->assigned_total = 0;
            }
        }
    }

    const sec duration = clock::now() - before; //Getting clustering time

    //Now moving on to output.
    if (assigner == "Classic"){
        myLogFile << "Algorithm: Lloyds" << endl;
    }
    else if (assigner == "Hypercube"){
        myLogFile << "Algorithm: Range Search Hypercube" << endl;
    }
    else{
        myLogFile << "Algorithm: Range Search LSH" << endl;
    }

    for (int k1 = 0; k1 < clusters; k1++){

        myLogFile << "CLUSTER-" << (k1 + 1) << " {";
        int size = kmeans_obj.ClusterArray[k1]->cluster_elements.size();
        myLogFile << "size: " << size << ", centroid: [";
        kmeans_obj.ClusterArray[k1]->centroid->displayVectorElementArray();
        myLogFile << "]" << endl;
    }
    myLogFile << "clustering_time: " << duration.count() << "[s]" << endl;

    //Getting silhouette scores for every cluster and a total one.
    double silhouette_total = kmeans_obj.silhouette(how_many_rows);
    myLogFile << "Silhouette: [";
    for (int k1 = 0; k1 < clusters; k1++){
        
        double get_sil = kmeans_obj.ClusterArray[k1]->silhouette_cluster;
        if (get_sil == -2){
            myLogFile << "Silhouette is undefined, ";
            continue;
        }
        
        myLogFile << "s" << (k1 + 1) << ": " << get_sil << ", ";
    }
    myLogFile << "stotal: " << silhouette_total << "]" << endl;

    //Print all ids, if complete parameter is set.
    if (complete){
        for (int k1 = 0; k1 < clusters; k1++){

            myLogFile << "CLUSTER-" << (k1 + 1) << " { centroid: [";
            kmeans_obj.ClusterArray[k1]->centroid->displayVectorElementArray();
            myLogFile << "], items: ";

            list<VectorElement *>::iterator hitr1;
            for (hitr1 = kmeans_obj.ClusterArray[k1]->cluster_elements.begin(); hitr1 != kmeans_obj.ClusterArray[k1]->cluster_elements.end(); ++hitr1){

                VectorElement *vobg = *hitr1;
                myLogFile << vobg->id << ", ";
            }
            myLogFile << "}" << endl;
        }
    }

    //---DELETE MEMORY---

    for (int i = 0; i < how_many_rows; i++)
    {
        delete Input_Array[i];
    }
    delete[] Input_Array;

    if (kmeans_obj.assigner == "Hypercube")
    {
        delete kmeans_obj.KMeans_Hyper;
    }
    else if (kmeans_obj.assigner == "LSH")
    {
        delete[] r_array;
        for (int i = 0; i < NUMBER_OF_HASH_TABLES; i++)
        {
            delete kmeans_obj.KMeans_Hash_Array[i];
        }
        delete[] kmeans_obj.KMeans_Hash_Array;
    }

    myLogFile.close();

    cout << "Program has successfully completed and written its results to the output file." << endl;
    return 0;
}