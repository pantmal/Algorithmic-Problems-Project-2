#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <chrono>
#include <random>
#include <algorithm> 
#include <float.h>
#include <math.h>  

#include "KMeans.h"
#include "Cluster.h"
#include "VectorElement.h"
#include "Helpers.h"

//KMeans constructor setting assigner method and cluster num
KMeans::KMeans(string assigner_arg, string updater_arg, int clusters_arg){


    this->assigner = assigner_arg;
    this->updater = updater_arg;
    this->clusters = clusters_arg;
}

//Initialization++ method
void KMeans::initialization(VectorElement** Input_Array, int input_size){


    //Pick first element random.

    int t_counter = 0;
    unsigned seed = chrono::steady_clock::now().time_since_epoch().count();
    default_random_engine e(seed);
    uniform_int_distribution<> U(0,input_size-1);

    VectorElement **init_centroids = new VectorElement *[clusters];
    this->ClusterArray = new Cluster *[clusters];
    
    int random_element = U(e);
    VectorElement* v_obg = Input_Array[random_element]; 
    init_centroids[t_counter] = v_obg;
    ClusterArray[t_counter] = new Cluster(init_centroids[t_counter],t_counter);

    //Now for the rest centroids...    
    while (t_counter != clusters-1){
        
        //Set up a P array and one for the candidates.
        int p_size = input_size-(t_counter+1);
        double* p_array = new double[p_size];
        int p_counter = 0;
        VectorElement** centroid_candidates = new VectorElement*[p_size];
        
        double curr_sum = 0.0;

        //Get maxiD(i) dist for normalization.
        double max_dist_all = -1.0;
        for (int i = 0; i < input_size; i++){
            bool is_centroid = false;
            for (int c = 0; c < (t_counter+1); c++){
                if (Input_Array[i]->id == init_centroids[c]->id){
                    is_centroid = true;
                }
            }

            if (is_centroid) continue;

            double min_dist = DBL_MAX;
            int min_id = -1;
            for (int k = 0; k < (t_counter+1); k++){

                VectorElement *vobj = Input_Array[i];
                vobj->getL2Distance(ClusterArray[k]->centroid);

                if (vobj->distanceCurrQ > min_dist){
                    min_dist = vobj->distanceCurrQ;
                } 
            }

            if (min_dist > max_dist_all){
                max_dist_all = min_dist;
            }

        }

        //Now create the P array
        for (int i = 0; i < input_size; i++){
            bool is_centroid = false;
            for (int c = 0; c < (t_counter+1); c++){
                if (Input_Array[i]->id == init_centroids[c]->id){
                    is_centroid = true;
                }
            }

            if (is_centroid) continue;

            double min_dist = DBL_MAX;
            int min_id = -1;
            for (int k = 0; k < (t_counter+1); k++){

                VectorElement *vobj = Input_Array[i];
                vobj->getL2Distance(ClusterArray[k]->centroid);

                if (vobj->distanceCurrQ < min_dist){
                    min_dist = vobj->distanceCurrQ;
                } 
            }


            min_dist = min_dist / max_dist_all; //Normalize the D
            double dist = pow(min_dist,2);


            curr_sum += dist;
            p_array[p_counter] = curr_sum; //Add the sum of D to the P array.
            centroid_candidates[p_counter] = Input_Array[i];
            p_counter++;

        }

        //Now get a x using uniform dist
        uniform_real_distribution<> Ux(0.0, curr_sum);
        double x = Ux(e);

        //Get the r from the P array with binary search
        int index = binarySearch(p_array,x,p_size);
        
        //And finally pick the new centroid and add it to the Cluster obj and array.
        t_counter++;
        VectorElement* v_obg = centroid_candidates[index]; 
        init_centroids[t_counter] = v_obg;
        ClusterArray[t_counter] = new Cluster(init_centroids[t_counter],t_counter);        


        delete[] centroid_candidates;
        delete[] p_array;
    }

    delete[] init_centroids;

}

//Assignment using Lloyd's. For every item pick nearest centroid.
void KMeans::ClassicAssignment(VectorElement** Input_Array, int how_many_rows){

    for (int i = 0; i < how_many_rows; i++){
        
        double min_dist = DBL_MAX;
        int min_id = -1;
        for (int k = 0; k < clusters; k++){

            VectorElement *vobj = Input_Array[i];
            vobj->getL2Distance(ClusterArray[k]->centroid);

            if (vobj->distanceCurrQ < min_dist){
                min_dist = vobj->distanceCurrQ;
                min_id = ClusterArray[k]->id;
            } 
        }

        ClusterArray[min_id]->cluster_elements.push_back(Input_Array[i]);
    }

}

//Reverse assignment for LSH and Hypercube
void KMeans::ReverseAssignment(VectorElement** Input_Array, int how_many_rows){


    //Find min dist among centroids
    double min_dist = DBL_MAX;
    for (int k = 0; k < clusters; k++){
        
        for (int k2 = 0; k2 < clusters; k2++){
            if (ClusterArray[k2]->id == ClusterArray[k]->id) continue;

            VectorElement* vobj = ClusterArray[k]->centroid;

            vobj->getL2Distance(ClusterArray[k2]->centroid);
            if (vobj->distanceCurrQ < min_dist){
                min_dist = vobj->distanceCurrQ;
            } 
        }
    }

    
    int assigned_total = 0;
    int non_assignemnts = 0;
    double power = 1;
    while (assigned_total != how_many_rows){ //Stop if all element are assigned
        
        for (int k = 0; k < clusters; k++){

            int size_before = ClusterArray[k]->cluster_elements.size();

            if (this->assigner == "Hypercube"){ //Add elements with Hypercube
                this->KMeans_Hyper->current_cluster = ClusterArray[k]->id; 
                this->KMeans_Hyper->range = min_dist * power; 
                this->KMeans_Hyper->range_list = ClusterArray[k]->cluster_elements;
                this->KMeans_Hyper->getFirstProbe(ClusterArray[k]->centroid, 0, "range");
                
                ClusterArray[k]->cluster_elements = this->KMeans_Hyper->range_list;
            }else{ //Add elements with LSH
                for (int j = 0; j < hashes; j++)
                {

                    if (j!=0){
                        this->KMeans_Hash_Array[j]->range_list = this->KMeans_Hash_Array[j-1]->range_list;        
                        this->KMeans_Hash_Array[j]->assigned_total = this->KMeans_Hash_Array[j-1]->assigned_total;      
                    }else{
                        this->KMeans_Hash_Array[j]->range_list = ClusterArray[k]->cluster_elements;
                    }

                    this->KMeans_Hash_Array[j]->current_cluster = ClusterArray[k]->id;
                    this->KMeans_Hash_Array[j]->RangeSearch(ClusterArray[k]->centroid, this->r_array, 0, min_dist * power);            
                }
                ClusterArray[k]->cluster_elements = this->KMeans_Hash_Array[hashes-1]->range_list;
            }

            //If size of cluster is the same after assignment methods, increment a counter. Will stop if two many clusters went unassigned.
            int size_after = ClusterArray[k]->cluster_elements.size();
            if (size_before == size_after){
                non_assignemnts++;
            }

        }

        //Check if there were double (or more) assignments for each item        
        for (int j = 0; j < how_many_rows; j++){

            if (Input_Array[j]->assigned_clusters.size() > 1){

                //Find the closest cluster of the assigned ones.
                double min_dist = DBL_MAX;
                int min_id = -1;
                for (list<int>::iterator i=Input_Array[j]->assigned_clusters.begin(); i!=Input_Array[j]->assigned_clusters.end(); i++){
                    int cluster_index = *i;
                    VectorElement *vobj = Input_Array[j];
                    vobj->getL2Distance(ClusterArray[cluster_index]->centroid);
                    if (vobj->distanceCurrQ < min_dist){
                        min_dist = vobj->distanceCurrQ;
                        min_id = ClusterArray[cluster_index]->id;
                    } 
            
                }   

                //And remove it from the other clusters.
                for (list<int>::iterator i=Input_Array[j]->assigned_clusters.begin(); i!=Input_Array[j]->assigned_clusters.end(); i++){
                    
                    int cluster_index = *i;
                    if (cluster_index == min_id) continue;
                    
                    ClusterArray[cluster_index]->cluster_elements.remove(Input_Array[j]);
                }
            }
            
        }

        //Reset values
        if (this->assigner == "Hypercube"){
            assigned_total = this->KMeans_Hyper->assigned_total;
        }else{
            assigned_total = this->KMeans_Hash_Array[hashes-1]->assigned_total;
        }

        power *= 2; //Mult the power

        if (non_assignemnts > (clusters/2)){ //Stop the assignment step if more than half of the clusters didn't get any new items.
            break;
        }

    }

    //Do the Classic assignment for the unassigned elems.
    int count = 0;
    if (assigned_total != how_many_rows){
        for (int i = 0; i < how_many_rows; i++){
            if (Input_Array[i]->assigned == false){
                
                double min_dist = DBL_MAX;
                int min_id = -1;
                for (int k = 0; k < clusters; k++){

                    VectorElement *vobj = Input_Array[i];
                    vobj->getL2Distance(ClusterArray[k]->centroid);

                    if (vobj->distanceCurrQ < min_dist){
                        min_dist = vobj->distanceCurrQ;
                        min_id = ClusterArray[k]->id;
                    } 
                }
                ClusterArray[min_id]->cluster_elements.push_back(Input_Array[i]);
            }
        }
    }
    
}

//Update step by getting the mean of the centroid.
void KMeans::update_vec(int columns){
    
    //Add up the column vals
    for (int k = 0; k < clusters; k++){

        for (int j = 0; j < columns; j++){

            double curr_sum = 0.0;
            list<VectorElement *>::iterator hitr1;
            for (hitr1 = ClusterArray[k]->cluster_elements.begin(); hitr1 != ClusterArray[k]->cluster_elements.end(); ++hitr1)
            {
                VectorElement *vobg = *hitr1;
                curr_sum += vobg->arrayVectorElement[j];
                
            }
            ClusterArray[k]->centroid->arrayVectorElement[j] = curr_sum;
        }

    }

    //And divide by list size
    for (int k = 0; k < clusters; k++){
        
        int list_size = ClusterArray[k]->cluster_elements.size();
        for (int j = 0; j < columns; j++){

            double mean = ClusterArray[k]->centroid->arrayVectorElement[j] / list_size;
            ClusterArray[k]->centroid->arrayVectorElement[j] = mean;
        }
    }



}

//Silhouette for the evaluation.
double KMeans::silhouette(int rows){


    if (clusters == 1){ //Edge case for only one cluster
        double silhouette_total = 0;
        return silhouette_total;
    }

    double silhouette_total = 0.0;
    for (int k = 0; k < clusters; k++){

        double silhouette_in_cluster = 0.0;

        int list_size = ClusterArray[k]->cluster_elements.size();
        int list_size_B = 0;

        //for every element of the cluster
        for (list<VectorElement *>::iterator hitr1 = ClusterArray[k]->cluster_elements.begin(); hitr1 != ClusterArray[k]->cluster_elements.end(); ++hitr1){
            VectorElement *curr_element = *hitr1;
            
            //Calculate the a(i)
            double a_dists = 0.0;
            for (list<VectorElement *>::iterator hitr2 = ClusterArray[k]->cluster_elements.begin(); hitr2 != ClusterArray[k]->cluster_elements.end(); ++hitr2){
                VectorElement *vobg2 = *hitr2;
                if (vobg2->id == curr_element->id) continue;

                curr_element->getL2Distance(vobg2);
                a_dists += curr_element->distanceCurrQ;
            }
            a_dists = a_dists / list_size;
        
            //Now find the 2nd closest cluster
            double min_dist = DBL_MAX;
            int min_id = -1;
            for (int k2 = 0; k2 < clusters; k2++){
                if (ClusterArray[k2]->id == ClusterArray[k]->id) continue;

                curr_element->getL2Distance(ClusterArray[k2]->centroid);
                if (curr_element->distanceCurrQ < min_dist){
                    min_dist = curr_element->distanceCurrQ;
                    min_id = ClusterArray[k2]->id;
                }
            }

            //And get the b(i)
            double b_dists = 0.0;
            list_size_B = ClusterArray[min_id]->cluster_elements.size();
            for (list<VectorElement *>::iterator hitr3 = ClusterArray[min_id]->cluster_elements.begin(); hitr3 != ClusterArray[min_id]->cluster_elements.end(); ++hitr3){
                VectorElement *vobg3 = *hitr3;
                curr_element->getL2Distance(vobg3);
                b_dists += curr_element->distanceCurrQ;
            }
            b_dists = b_dists / list_size_B;
        
            //Now get the silhouett score
            double max_of_ab = max(a_dists,b_dists);
            double diff = b_dists - a_dists;
            double silhouette_curr = diff/max_of_ab;

            curr_element->silhouette_score = silhouette_curr;

            silhouette_in_cluster += silhouette_curr;
            silhouette_total += silhouette_curr;
        }
        
        if (list_size <= 1 || list_size_B <= 1){ //Edge case when a cluster has only one element
            ClusterArray[k]->silhouette_cluster = -2;
        }else{
            ClusterArray[k]->silhouette_cluster = silhouette_in_cluster / list_size; //Otherwise set the mean silhouette for this cluster.
        }
        
    }

    //Return the silhouette mean total
    silhouette_total = silhouette_total / rows; 
    return silhouette_total;
}

//KMeans destructor
KMeans::~KMeans(){

    for (int i = 0; i < clusters; i++){
        delete ClusterArray[i];
    }

    delete[] ClusterArray;
}