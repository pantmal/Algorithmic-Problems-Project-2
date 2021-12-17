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
#include "CurveElement.h"
#include "DiscreteFrechet.h"
#include "Helpers.h"
#include "TreeNode.h"

//KMeans constructor setting assigner method and cluster num
KMeans::KMeans(string assigner_arg, string updater_arg, int clusters_arg){


    this->assigner = assigner_arg;
    this->updater = updater_arg;
    this->clusters = clusters_arg;
}

//Initialization++ method
void KMeans::initialization(int input_size, VectorElement** Input_Array){


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

void KMeans::initialization(int input_size, CurveElement** Input_Array_Frechet){

    int t_counter = 0;
    unsigned seed = chrono::steady_clock::now().time_since_epoch().count();
    default_random_engine e(seed);
    uniform_int_distribution<> U(0,input_size-1);

    this->ClusterArray = new Cluster *[clusters];
    int random_element = U(e);

    CurveElement **init_centroids = new CurveElement *[clusters];
    CurveElement* v_obg = Input_Array_Frechet[random_element]; 
    init_centroids[t_counter] = v_obg;
    ClusterArray[t_counter] = new Cluster(init_centroids[t_counter],t_counter);
    

    //Now for the rest centroids...    
    while (t_counter != clusters-1){
        
        //Set up a P array and one for the candidates.
        int p_size = input_size-(t_counter+1);
        double* p_array = new double[p_size];
        int p_counter = 0;
        CurveElement** centroid_candidates = new CurveElement*[p_size];
        
        double curr_sum = 0.0;

        //Get maxiD(i) dist for normalization.
        double max_dist_all = -1.0;
        for (int i = 0; i < input_size; i++){
            bool is_centroid = false;
            for (int c = 0; c < (t_counter+1); c++){
                if (Input_Array_Frechet[i]->id == init_centroids[c]->id){
                    is_centroid = true;
                }
            }

            if (is_centroid) continue;

            double min_dist = DBL_MAX;
            int min_id = -1;
            for (int k = 0; k < (t_counter+1); k++){

                CurveElement *vobj = Input_Array_Frechet[i];
                double d = vobj->arrayElementTwoD.size();
                double dfd = ret_DFD(d,d,vobj->arrayElementTwoD,ClusterArray[k]->centroid_frechet->arrayElementTwoD);
                
                if (dfd > min_dist){
                    min_dist = dfd;
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
                if (Input_Array_Frechet[i]->id == init_centroids[c]->id){
                    is_centroid = true;
                }
            }

            if (is_centroid) continue;

            double min_dist = DBL_MAX;
            int min_id = -1;
            for (int k = 0; k < (t_counter+1); k++){

                CurveElement *vobj = Input_Array_Frechet[i];
                double d = vobj->arrayElementTwoD.size();
                double dfd = ret_DFD(d,d,vobj->arrayElementTwoD,ClusterArray[k]->centroid_frechet->arrayElementTwoD);

                if (dfd < min_dist){
                    min_dist = dfd;
                } 
            }


            min_dist = min_dist / max_dist_all; //Normalize the D
            double dist = pow(min_dist,2);


            curr_sum += dist;
            p_array[p_counter] = curr_sum; //Add the sum of D to the P array.
            centroid_candidates[p_counter] = Input_Array_Frechet[i];
            p_counter++;

        }

        //Now get a x using uniform dist
        uniform_real_distribution<> Ux(0.0, curr_sum);
        double x = Ux(e);

        //Get the r from the P array with binary search
        int index = binarySearch(p_array,x,p_size);
        
        //And finally pick the new centroid and add it to the Cluster obj and array.
        t_counter++;
        CurveElement* v_obg = centroid_candidates[index]; 
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


void KMeans::ClassicAssignment(CurveElement** Input_Array_Frechet, int how_many_rows){

    for (int i = 0; i < how_many_rows; i++){
        
        double min_dist = DBL_MAX;
        int min_id = -1;
        for (int k = 0; k < clusters; k++){

            CurveElement *vobj = Input_Array_Frechet[i];
            double d = vobj->arrayElementTwoD.size();
            double dfd = ret_DFD(d,d,vobj->arrayElementTwoD,ClusterArray[k]->centroid_frechet->arrayElementTwoD);

            if (dfd < min_dist){
                min_dist = dfd;
                min_id = ClusterArray[k]->id;
            } 
        }

        ClusterArray[min_id]->frechet_elements.push_back(Input_Array_Frechet[i]);
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
                    this->KMeans_Hash_Array[j]->RangeSearch(ClusterArray[k]->centroid, this->r_array, 0, min_dist * power, "LSH");            
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

void KMeans::ReverseAssignment(CurveElement** Input_Array, int how_many_rows, int how_many_columns, double delta, int M){


    //Find min dist among centroids
    double min_dist = DBL_MAX;
    for (int k = 0; k < clusters; k++){
        
        for (int k2 = 0; k2 < clusters; k2++){
            if (ClusterArray[k2]->id == ClusterArray[k]->id) continue;

            CurveElement* vobj = ClusterArray[k]->centroid_frechet;


            double d = vobj->arrayElementTwoD.size();
            double dfd = ret_DFD(d,d,vobj->arrayElementTwoD,ClusterArray[k2]->centroid_frechet->arrayElementTwoD);

            if (dfd < min_dist){
                min_dist = dfd;
            } 
        }
    }

    
    int assigned_total = 0;
    int non_assignemnts = 0;
    double power = 1;
    while (assigned_total != how_many_rows){ //Stop if all element are assigned
        
        for (int k = 0; k < clusters; k++){

            int size_before = ClusterArray[k]->frechet_elements.size();

            for (int j = 0; j < hashes; j++)
            {
                if (j!=0){
                    this->KMeans_Hash_Array[j]->range_list_frechet = this->KMeans_Hash_Array[j-1]->range_list_frechet;        
                    this->KMeans_Hash_Array[j]->assigned_total = this->KMeans_Hash_Array[j-1]->assigned_total;      
                }else{
                    this->KMeans_Hash_Array[j]->range_list_frechet = ClusterArray[k]->frechet_elements;
                }

                this->KMeans_Hash_Array[j]->current_cluster = ClusterArray[k]->id;

                double t1 = this->KMeans_Hash_Array[j]->t1;
                double t2 = this->KMeans_Hash_Array[j]->t2;
                ClusterArray[k]->centroid_frechet->Snapping2d(t1,t2,delta,how_many_columns);
                string str_curve = ClusterArray[k]->centroid_frechet->Vectorization2d(how_many_columns,M);
                
                VectorElement* vec2add = new VectorElement(how_many_columns*2, str_curve, hashes);
                vec2add->original_curve = ClusterArray[k]->centroid_frechet;

                this->KMeans_Hash_Array[j]->RangeSearch(vec2add, this->r_array, 0, min_dist * power, "LSH_Frechet");
                delete vec2add;                
            }
            ClusterArray[k]->frechet_elements = this->KMeans_Hash_Array[hashes-1]->range_list_frechet;
            

            //If size of cluster is the same after assignment methods, increment a counter. Will stop if two many clusters went unassigned.
            int size_after = ClusterArray[k]->frechet_elements.size();
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

                    CurveElement *vobj = Input_Array[j];
                    
                    double d = vobj->arrayElementTwoD.size();
                    double dfd = ret_DFD(d,d,vobj->arrayElementTwoD,ClusterArray[cluster_index]->centroid_frechet->arrayElementTwoD);
                    
                    if (dfd < min_dist){
                        min_dist = dfd;
                        min_id = ClusterArray[cluster_index]->id;
                    } 
            
                }   

                //And remove it from the other clusters.
                for (list<int>::iterator i=Input_Array[j]->assigned_clusters.begin(); i!=Input_Array[j]->assigned_clusters.end(); i++){
                    
                    int cluster_index = *i;
                    if (cluster_index == min_id) continue;
                    
                    ClusterArray[cluster_index]->frechet_elements.remove(Input_Array[j]);
                }
            }
            
        }

        
        assigned_total = this->KMeans_Hash_Array[hashes-1]->assigned_total;
        
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

                    CurveElement *vobj = Input_Array[i];
                    double d = vobj->arrayElementTwoD.size();
                    double dfd = ret_DFD(d,d,vobj->arrayElementTwoD,ClusterArray[k]->centroid_frechet->arrayElementTwoD);

                    if (dfd < min_dist){
                        min_dist = dfd;
                        min_id = ClusterArray[k]->id;
                    } 
                }
                ClusterArray[min_id]->frechet_elements.push_back(Input_Array[i]);
            }
        }
    }
    
}

//Update step by getting the mean of the centroid.
void KMeans::update_vec(int columns){
    
    //Add up the column vals
    for (int k = 0; k < clusters; k++){

        int list_size = ClusterArray[k]->cluster_elements.size();
        
        if (list_size == 0){
            continue;
        }

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

        if (list_size == 0){
            continue;
        }
        
        for (int j = 0; j < columns; j++){

            double mean = ClusterArray[k]->centroid->arrayVectorElement[j] / list_size;
            ClusterArray[k]->centroid->arrayVectorElement[j] = mean;
        }
    }



}

void KMeans::update_curve(){

    
    unsigned seed = chrono::steady_clock::now().time_since_epoch().count();
    default_random_engine e(seed);


    for (int k = 0; k < clusters; k++){

        if(ClusterArray[k]->centroid_frechet->mark_deletion){
            delete ClusterArray[k]->centroid_frechet;
            ClusterArray[k]->centroid_frechet = NULL;
        }
       
        int list_size = ClusterArray[k]->frechet_elements.size();
        if (list_size == 0){
            continue;
        }

        double log_res = log2(list_size);
        int height = ceil(log_res);

        int tree_size = pow(2,height+1) - 1;
        int max_leaves = pow(2,height);
        
        if (max_leaves != list_size){
            int removed_nodes = max_leaves - list_size;
            tree_size = tree_size - removed_nodes;
        }

        //cout << "here " << list_size <<" " << height << " " << tree_size << endl;
        TreeNode* MeanCurveTree = MeanCurveTree->AddNode(MeanCurveTree,0,tree_size);
        
        list<CurveElement *> cluster_copy = ClusterArray[k]->frechet_elements;

        while(!cluster_copy.empty()){
            //cout << "ll size " << ll.size() << endl;
            uniform_int_distribution<> U(0, cluster_copy.size() - 1);
            int random_element = U(e);
            
            //Find the item in the list and remove it. We only check each probe once.
            list<CurveElement* >::iterator it = cluster_copy.begin();
            advance(it, random_element);
            CurveElement* item = *it;
            cluster_copy.erase(it);
            
            MeanCurveTree->AddCurve(MeanCurveTree,item); 
        }

        //cout << "here f" << endl;

        CurveElement* final_mean = MeanCurveTree->MeanCurveTraversal(MeanCurveTree);
        //final_mean->displayVectorElementGrid();
        //myLogFile << "size f" << final_mean->arrayElementTwoD.size() << endl;
        //cout << "here f" << endl;
        
        ClusterArray[k]->centroid_frechet = final_mean;

        MeanCurveTree->DeleteNode(MeanCurveTree->left);
        MeanCurveTree->DeleteNode(MeanCurveTree->right);
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

double KMeans::silhouette_frechet(int rows){
    
    
    if (clusters == 1){ //Edge case for only one cluster
        double silhouette_total = -2;
        return silhouette_total;
    }

    double silhouette_total = 0.0;
    for (int k = 0; k < clusters; k++){

        double silhouette_in_cluster = 0.0;

        int list_size = ClusterArray[k]->frechet_elements.size();
        int list_size_B = 0;

        //for every element of the cluster
        for (list<CurveElement *>::iterator hitr1 = ClusterArray[k]->frechet_elements.begin(); hitr1 != ClusterArray[k]->frechet_elements.end(); ++hitr1){
            CurveElement *curr_element = *hitr1;
            
            //Calculate the a(i)
            double a_dists = 0.0;
            for (list<CurveElement *>::iterator hitr2 = ClusterArray[k]->frechet_elements.begin(); hitr2 != ClusterArray[k]->frechet_elements.end(); ++hitr2){
                
                CurveElement *vobg2 = *hitr2;
                if (vobg2->id == curr_element->id) continue;
                
                double d = curr_element->arrayElementTwoD.size();
                double dfd = ret_DFD(d,d,curr_element->arrayElementTwoD,vobg2->arrayElementTwoD);
                a_dists += dfd;
            }
            a_dists = a_dists / list_size;
        
            //Now find the 2nd closest cluster
            double min_dist = DBL_MAX;
            int min_id = -1;
            for (int k2 = 0; k2 < clusters; k2++){
                if (ClusterArray[k2]->id == ClusterArray[k]->id) continue;

                double d = curr_element->arrayElementTwoD.size();
                double dfd = ret_DFD(d,d,curr_element->arrayElementTwoD,ClusterArray[k2]->centroid_frechet->arrayElementTwoD);

                if (dfd < min_dist){
                    min_dist = dfd;
                    min_id = ClusterArray[k2]->id;
                }
            }

            //And get the b(i)
            double b_dists = 0.0;
            list_size_B = ClusterArray[min_id]->frechet_elements.size();
            //cout << "listB " << list_size_B << endl;
            for (list<CurveElement *>::iterator hitr3 = ClusterArray[min_id]->frechet_elements.begin(); hitr3 != ClusterArray[min_id]->frechet_elements.end(); ++hitr3){
                CurveElement *vobg3 = *hitr3;
                
                double d = curr_element->arrayElementTwoD.size();
                double dfd = ret_DFD(d,d,curr_element->arrayElementTwoD,vobg3->arrayElementTwoD);
                b_dists += dfd;
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