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

#include "VectorElement.h"
#include "CurveElement.h"
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
    double delta = -1.0;            //delta for frechet
    bool complete = false;          //complete clusters, optional
    bool silhouette_param = false;  //silhouette results, optional
    string assigner = "Classic"; //assignment method. Will use Classic if none provided.
    string updater = "vector"; //update step. Will use vector if none provided.

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
            complete = true;
        }
        else if (strcmp(argv[i], "-delta") == 0)
        {
            string tempd;
            tempd = argv[i + 1];
            delta = args_string_to_double(tempd);
        }
        else if (strcmp(argv[i], "-silhouette") == 0)
        {
            silhouette_param = true;
        }
        else if (strcmp(argv[i], "-update") == 0)
        {

            if (strcmp(argv[i + 1], "Mean") != 0){
                cout << "Method must be Mean Frechet or Mean Vector - wrong input" << endl;
                exit(0);
            }

            if (strcmp(argv[i + 2], "Frechet") == 0)
            {
                updater = "frechet";
            }
            else if (strcmp(argv[i + 2], "Vector") == 0)
            {
                updater = "vector";
            }
            else
            {
                cout << "Method must be Mean Frechet or Mean Vector - wrong input" << endl;
                exit(0);
            }
        }
        else if (strcmp(argv[i], "-assignment") == 0)
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
            else if (strcmp(argv[i + 1], "LSH_Frechet") == 0)
            {
                assigner = "LSH_Frechet";
            }
            else
            {
                cout << "Method must be Classic or LSH or Hypercube or LSH_Frechet - wrong input" << endl;
                exit(0);
            }
        }
    }

    //Rejecting incompatible combos
    if (updater == "vector" && assigner == "LSH_Frechet"){
        cout << "Mean Vector update step is incompatible with LSH_Frechet assignment step. Choose a new combination." << endl;
        exit(0);
    }

    if (updater == "frechet" && (assigner == "LSH" || assigner == "Hypercube")){
        cout << "Mean Frechet update step is incompatible with LSH or Hypercube assignment steps. Choose a new combination." << endl;
        exit(0);
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
    
    if (NUMBER_OF_HASH_TABLES == -1) NUMBER_OF_HASH_TABLES = 3; //PARAM <L>
    
    if (k_input == -1) k_input = 4; //PARAM <k in LSH>
    
    if (M == -1) M = 10 ; //PARAM <radius>
    
    if (kdim == -1) kdim = 3; //PARAM <cube dims>
    
    if (probes == -1) probes = 2; //PARAM  <probes>
    
    if (delta == -1.0) delta = 1; //PARAM <d>
    
    bool justOnce = true;
    int how_many_columns = 0;
    int how_many_rows = 0;
    string temp;
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
            string comma = ",";
            if (mystring.find(comma) != std::string::npos)
            {
                // cout << "removing the commas" << endl;
                replace(mystring.begin(), mystring.end(), ',', ' ');
            }
            stringstream sso(mystring);
            sso >> temp;
            while (justOnce && sso >> tempString)
            {
                how_many_columns++; //calculate the number of columns (dimension of the vector/curve without the id)
            }
            justOnce = false;
        }
    }
    myfile.close();
    myfile.clear();

    //And now initialize the objects.
    VectorElement **Input_Array;
    CurveElement **Input_Array_Frechet;

    if (updater == "vector"){
        myfile.open(FILE_NAME_INPUT);
        Input_Array = new VectorElement *[how_many_rows];
        int i = 0;
        if (myfile.is_open())
        {
            while (myfile)
            {
                getline(myfile, mystring);
                string comma = ",";
                if (mystring.find(comma) != std::string::npos)
                {
                    // cout << "removing the commas" << endl;
                    replace(mystring.begin(), mystring.end(), ',', ' ');
                }
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
    }else{

        myfile.open(FILE_NAME_INPUT);
        Input_Array_Frechet = new CurveElement *[how_many_rows];
        int i = 0;
        if (myfile.is_open())
        {
            while (myfile)
            {
                getline(myfile, mystring);
                string comma = ",";
                if (mystring.find(comma) != std::string::npos)
                {
                    // cout << "removing the commas" << endl;
                    replace(mystring.begin(), mystring.end(), ',', ' ');
                }
                stringstream sso(mystring);
                if (i < how_many_rows)
                {
                    Input_Array_Frechet[i] = new CurveElement(how_many_columns, mystring, 0,"discrete");
                    i++;
                }
            }
        }
        myfile.close();
        myfile.clear();

    }


    //cout << "columns==== " << how_many_columns << endl;
    //cout << "rows==== " << how_many_rows << endl;
    //--------DATA COLLECTED----------
    
    int NUMBER_OF_BUCKETS = how_many_rows / 8;

    int N = 0; //N is unused. It is set to 0 because it is a HyperCube constructor argument.
    int w = 400; //Hardcoded value for w.

    //KMeans object is constructed and its centroid are initialized.
    KMeans kmeans_obj(assigner, updater,clusters);
    if (updater == "vector"){
        kmeans_obj.initialization(how_many_rows, Input_Array);
    }else{
        kmeans_obj.initialization(how_many_rows, Input_Array_Frechet);
    }
    

    //Setting up the HyperCube and LSHash objects if need be.
    int *r_array;

    VectorElement **vectorized_input_storage;
    if (kmeans_obj.assigner == "Hypercube")
    {
        kmeans_obj.KMeans_Hyper = new HyperCube(kdim, how_many_columns, w, N, M, probes, 0.0);
        kmeans_obj.KMeans_Hyper->cluster_mode = true;

        for (int j = 0; j < how_many_rows; j++)
        {
            kmeans_obj.KMeans_Hyper->insertItem(Input_Array[j]);
        }
    }
    else if (kmeans_obj.assigner == "LSH" || kmeans_obj.assigner == "LSH_Frechet")
    {
        kmeans_obj.KMeans_Hash_Array = new LSHash *[NUMBER_OF_HASH_TABLES];
        kmeans_obj.hashes = NUMBER_OF_HASH_TABLES;

        for (int i = 0; i < NUMBER_OF_HASH_TABLES; i++)
        {
            if (kmeans_obj.assigner == "LSH"){
                kmeans_obj.KMeans_Hash_Array[i] = new LSHash(NUMBER_OF_BUCKETS, how_many_columns, k_input, w);
            }else{
                kmeans_obj.KMeans_Hash_Array[i] = new LSHash(NUMBER_OF_BUCKETS, 2*how_many_columns, k_input, w);
            }
            kmeans_obj.KMeans_Hash_Array[i]->cluster_mode = true;
            kmeans_obj.KMeans_Hash_Array[i]->query_rows_field = 0;
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

        if (kmeans_obj.assigner == "LSH"){
            for (int i = 0; i < NUMBER_OF_HASH_TABLES; i++)
            {
                for (int j = 0; j < how_many_rows; j++)
                {
                    kmeans_obj.KMeans_Hash_Array[i]->insertItem(Input_Array[j], r_array);
                }
            }
        }else{

            vectorized_input_storage = new VectorElement *[how_many_rows*NUMBER_OF_HASH_TABLES];
            int vec_counter = 0;

            uniform_int_distribution<> UM(100000, INT_MAX-1000000);
            int M = UM(e);
            //cout << M << endl;

            for (int i = 0; i < NUMBER_OF_HASH_TABLES; i++)
            {
                //Getting t values
                double open_d = delta - 1;
                uniform_real_distribution<> U(0.0, open_d);
                double t1 = U(e);
                double t2 = U(e);
                kmeans_obj.KMeans_Hash_Array[i]->t1 = t1;
                kmeans_obj.KMeans_Hash_Array[i]->t2 = t2;

                for (int j = 0; j < how_many_rows; j++)
                {
                    //Grid and concat
                    Input_Array_Frechet[j]->Snapping2d(t1,t2,delta,how_many_columns);
                    string str_curve = Input_Array_Frechet[j]->Vectorization2d(how_many_columns,M);

                    //Getting curve as vector
                    VectorElement* vec2add = new VectorElement(how_many_columns*2, str_curve, NUMBER_OF_HASH_TABLES);
                    vec2add->original_curve = Input_Array_Frechet[j];
                    
                    //insert the key
                    kmeans_obj.KMeans_Hash_Array[i]->insertItem(vec2add, r_array);
                    
                    //store the vec for deletion
                    vectorized_input_storage[vec_counter] = vec2add;
                    vec_counter++;

                    //clear for next hash table
                    Input_Array_Frechet[j]->gridElementTwoD.clear();
                    //myLogFile << "END OF ARR" << endl;
                }
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
            if (kmeans_obj.updater == "vector"){
                kmeans_obj.ClassicAssignment(Input_Array, how_many_rows);
            }else{
                kmeans_obj.ClassicAssignment(Input_Array_Frechet, how_many_rows);
            }
            
        }
        else if (kmeans_obj.assigner == "Hypercube" || kmeans_obj.assigner == "LSH"){
            kmeans_obj.ReverseAssignment(Input_Array, how_many_rows);
        }else{
            kmeans_obj.ReverseAssignment(Input_Array_Frechet,how_many_rows,how_many_columns,delta,M);
        }

        //Update step
        if (kmeans_obj.updater == "vector"){
            kmeans_obj.update_vec(how_many_columns);
        }else{
            kmeans_obj.update_curve();
        }
        
        //Clearing up some values for the next pass.
        for (int j = 0; j < how_many_rows; j++){
            
            if (updater == "vector"){
                Input_Array[j]->assigned = false;
                Input_Array[j]->assigned_clusters.clear();
            }else{
                Input_Array_Frechet[j]->assigned = false;
                Input_Array_Frechet[j]->assigned_clusters.clear();
            }
            
        }
        if (i != (iterations-1)){
            
            if (updater == "vector"){
                for (int k1 = 0; k1 < clusters; k1++){
                    kmeans_obj.ClusterArray[k1]->cluster_elements.clear();
                }

                if (kmeans_obj.assigner == "Hypercube"){ 
                    kmeans_obj.KMeans_Hyper->assigned_total = 0;
                }
                else if (kmeans_obj.assigner == "LSH"){
                    kmeans_obj.KMeans_Hash_Array[0]->assigned_total = 0;
                }
            }else{
                for (int k1 = 0; k1 < clusters; k1++){
                    kmeans_obj.ClusterArray[k1]->frechet_elements.clear();
                }
                if (kmeans_obj.assigner == "LSH_Frechet"){
                    kmeans_obj.KMeans_Hash_Array[0]->assigned_total = 0;
                }
            }
            
        }
    }

    const sec duration = clock::now() - before; //Getting clustering time

    //Now moving on to output.
    if (assigner == "Classic"){
        myLogFile << "Algorithm: Lloyds "<< endl;
    }
    else if (assigner == "Hypercube"){
        myLogFile << "Algorithm: Range Search Hypercube "<< endl;
    }
    else if (assigner == "LSH"){
        myLogFile << "Algorithm: Range Search LSH Vector"<< endl;
    }else{
        myLogFile << "Algorithm: Range Search LSH Frechet"<< endl;
    }

    if (updater == "vector"){
        myLogFile << "Update: Mean Vector " << endl;
    }
    else{
        myLogFile << "Update: Mean Frechet " << endl;
    }

    myLogFile << endl;

    for (int k1 = 0; k1 < clusters; k1++){

        myLogFile << "CLUSTER-" << (k1 + 1) << " {";
        if (updater == "vector"){
            int size = kmeans_obj.ClusterArray[k1]->cluster_elements.size();
            myLogFile << "size: " << size << ", centroid: [";
            kmeans_obj.ClusterArray[k1]->centroid->displayVectorElementArray();
            myLogFile << "]" << endl;
            myLogFile << endl;   
        }else{
            int size = kmeans_obj.ClusterArray[k1]->frechet_elements.size();
            myLogFile << "size: " << size << ", centroid: [";
            kmeans_obj.ClusterArray[k1]->centroid_frechet->displayVectorElementArray2D();
            myLogFile << "]" << endl;   
            myLogFile << endl;
        }

        
    }
    myLogFile << "clustering_time: " << duration.count() << "[s]" << endl;
    myLogFile << endl;

    //Getting silhouette scores for every cluster and a total one.
    if (silhouette_param){
        
        double silhouette_total;
        if (updater == "vector"){
            silhouette_total = kmeans_obj.silhouette(how_many_rows);
        }else{
            silhouette_total = kmeans_obj.silhouette_frechet(how_many_rows);
        }
        
        
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
        myLogFile << endl;
    }
    

    //Print all ids, if complete parameter is set.
    if (complete){
        for (int k1 = 0; k1 < clusters; k1++){

            if(updater == "vector"){
                myLogFile << "CLUSTER-" << (k1 + 1) << " { centroid: [";
                kmeans_obj.ClusterArray[k1]->centroid->displayVectorElementArray();
                myLogFile << "], items: ";

                list<VectorElement *>::iterator hitr1;
                for (hitr1 = kmeans_obj.ClusterArray[k1]->cluster_elements.begin(); hitr1 != kmeans_obj.ClusterArray[k1]->cluster_elements.end(); ++hitr1){

                    VectorElement *vobg = *hitr1;
                    myLogFile << vobg->id << ", ";
                }
                myLogFile << "}" << endl;
                myLogFile << endl;
            }else{
                myLogFile << "CLUSTER-" << (k1 + 1) << " { centroid: [";
                kmeans_obj.ClusterArray[k1]->centroid_frechet->displayVectorElementArray2D();
                myLogFile << "], items: ";

                list<CurveElement *>::iterator hitr1;
                for (hitr1 = kmeans_obj.ClusterArray[k1]->frechet_elements.begin(); hitr1 != kmeans_obj.ClusterArray[k1]->frechet_elements.end(); ++hitr1){

                    CurveElement *vobg = *hitr1;
                    myLogFile << vobg->id << ", ";
                }
                myLogFile << "}" << endl;
                myLogFile << endl;
            }

            
        }
    }

    //---DELETE MEMORY---

    if (updater != "vector"){
        for (int k1 = 0; k1 < clusters; k1++){
            if(kmeans_obj.ClusterArray[k1]->centroid_frechet->mark_deletion){
                delete kmeans_obj.ClusterArray[k1]->centroid_frechet;
            }
        }
    }
    

    if (updater == "vector"){
        for (int i = 0; i < how_many_rows; i++)
        {
            delete Input_Array[i];
        }
        delete[] Input_Array;
    }else{
        for (int i = 0; i < how_many_rows; i++)
        {
            delete Input_Array_Frechet[i];
        }
        delete[] Input_Array_Frechet;
    }

    if (kmeans_obj.assigner == "Hypercube")
    {
        delete kmeans_obj.KMeans_Hyper;
    }
    else if (kmeans_obj.assigner == "LSH" || kmeans_obj.assigner == "LSH_Frechet")
    {
        delete[] r_array;
        for (int i = 0; i < NUMBER_OF_HASH_TABLES; i++)
        {
            delete kmeans_obj.KMeans_Hash_Array[i];
        }
        delete[] kmeans_obj.KMeans_Hash_Array;

        if (kmeans_obj.assigner == "LSH_Frechet"){
            for (int i = 0; i < how_many_rows*NUMBER_OF_HASH_TABLES; i++)
            {
                delete vectorized_input_storage[i];
            }
            delete[] vectorized_input_storage;
        }
        
    }
    
    
    myLogFile.close();

    cout << "Program has successfully completed and written its results to the output file." << endl;
    return 0;
}