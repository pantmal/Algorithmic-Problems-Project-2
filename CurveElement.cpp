#include <cmath>
#include <bits/stdc++.h>

#include "CurveElement.h"
#include "Helpers.h"

using namespace std;

//std::ofstream myLogFile;

CurveElement::CurveElement(int dimensions, std::string fileLine, int numberOfHashTables, std::string variant)
{

    assigned = false;
    mark_deletion = false;

    QueryTrickid = new unsigned int[numberOfHashTables]; //gets set when the curve element gets into an LSH bucket
    
    id = "0";
    double temp;
    int counter = 0;
    distanceCurrQ = 0;
    
    size = dimensions;
    
    std::stringstream sso(fileLine);
    sso >> id; //Get the id
    while (sso >> temp) //And the curve values in the array field
    {
        // arrayVectorElement[counter] = temp;
        if (variant == "discrete"){
            arrayElementTwoD.push_back({counter+1,temp});
            gridElementTwoD.push_back({0,0.0}); //init values to prevent seg faults
        }else{
            arrayElementOneD.push_back(temp);
            filteredElementOneD.push_back(0.0); //init values to prevent seg faults
            gridElementOneD.push_back({0.0}); //init values to prevent seg faults
        }
        counter++;
    }
    

}

//Overloaded constructor for Mean Curves
CurveElement::CurveElement(std::vector<std::tuple <double, double, double, double > >& traversal){

    
    mark_deletion = true;

    for (const auto &i : traversal){
        double Px = get<0>(i);
        double Py = get<1>(i);
        double Qx = get<2>(i);
        double Qy = get<3>(i);

        double mean_x = (Px + Qx) / 2;
        double mean_y = (Py + Qy) / 2;

        arrayElementTwoD.push_back({mean_x,mean_y});
    }


}

//Snapping for 2D curves
void CurveElement::Snapping2d(double t1, double t2, double delta, int columns){

    gridElementTwoD.clear();

    double prev_x = 0;
    double prev_y = 0;
    for (int j = 0; j < columns; j++){
        
        double x = get<0>( arrayElementTwoD[j]);
        double y = get<1>( arrayElementTwoD[j]);
        
        //apply snapping
        double grid_x = (floor((abs(x-t1)/delta) + 0.5) * delta) + t1; 
        double grid_y = (floor((abs(y-t2)/delta) + 0.5) * delta) + t2;
        
        //remove duplicates
        if (j != 0){
            if (prev_x != grid_x || prev_y != grid_y ){
                
                gridElementTwoD.push_back({grid_x,grid_y});
            }
            
        }else if (j == 0){
            gridElementTwoD.push_back({grid_x,grid_y});
        }

        prev_x = grid_x;
        prev_y = grid_y;
        
    }

    //displayVectorElementGrid();
}

//Vectorizing a 2D curve for LSH
string CurveElement::Vectorization2d(int columns, int M){

    //Concatenation using a vector
    vector<double>  VectorizedCurve;
    int grid_size = gridElementTwoD.size();
    for (int v = 0; v < grid_size ;v++ ){
        double x = get<0>( gridElementTwoD[v]);
        double y = get<1>( gridElementTwoD[v]);

        VectorizedCurve.push_back(x);
        VectorizedCurve.push_back(y);
        
    }

    //Padding
    int size_before_pad = VectorizedCurve.size();
    for (int pad_c = size_before_pad; pad_c < 2*columns; pad_c++){
        VectorizedCurve.push_back(M);
    }
    
    //VectorElement contructor needs string argument so we need to convert the key to string format.
    std::stringstream sso;
    sso << this->id;
    sso << " ";
    for(size_t i = 0; i < VectorizedCurve.size(); ++i){
        if(i != 0)
            sso << " ";
        sso << VectorizedCurve[i];
    }
    string str_curve = sso.str();

    return str_curve;

}

//Filtering for 1D curves
void CurveElement::Filtering(double e){

    
    filteredElementOneD.clear();
    vector<double> v = arrayElementOneD;
    
    auto it = v.begin();
    while (it != v.end()){
        
        //Stop if vector bounds exceeded
        if (it >= v.end() || it+1 >= v.end() || it+2 >= v.end()){
            break;
        }

        //Get three vals
        double a = *it;
        double b = *(it+1);
        double c = *(it+2);

        double res1 = abs(a-b);
        double res2 = abs(b-c);

        // myLogFile << "a b c "<< a << " " << b << " "<< c << endl;

        //Either erase the item or increase the pointer/counter
        if (res1 <= e && res2 <= e){
            v.erase(it+1);
        }else{
            it = it+1;
        }

    }

    filteredElementOneD = v;

}

//Snapping for 1D curves
void CurveElement::Snapping1d(double t, double delta){

    gridElementOneD.clear();

    int filt_size = filteredElementOneD.size(); 
    for (int j = 0; j < filt_size; j++ ){

        double y = filteredElementOneD[j];
        double grid_y = floor(abs(y)+t/delta) * delta; //apply snapping

        gridElementOneD.push_back(grid_y);
    }
}

//Getting Minima-Maxima of an 1D curve. Same logic as in filtering.
void CurveElement::MinMax(){

    vector<double> v2 = gridElementOneD;
    auto it2 = v2.begin();
    while (it2 != v2.end()){
        
        if (it2 >= v2.end() || it2+1 >= v2.end() || it2+2 >= v2.end()){
            break;
        }

        double a = *it2;
        double b = *(it2+1);
        double c = *(it2+2);

        double min_val = min(a,c);
        double max_val = max(a,c);

        // myLogFile << "a b c "<< a << " " << b << " "<< c << endl;

        if (min_val <= b && max_val >= b){
            v2.erase(it2+1);
        }else{
            it2 = it2+1;
        }

    }
    
    gridElementOneD = v2;

}

//Vectorizing an 1D curve for LSH
string CurveElement::Vectorization1d(int columns, int M){

    //Concatenation using a vector
    vector<double>  VectorizedCurve;
    int grid_size = gridElementOneD.size();
    for (int v = 0; v < grid_size ;v++ ){
        
        double y = gridElementOneD[v];
        VectorizedCurve.push_back(y);
    }

    //Padding
    int size_before_pad = VectorizedCurve.size();
    for (int pad_c = size_before_pad; pad_c < columns; pad_c++){
        VectorizedCurve.push_back(M);
    }
    
    //VectorElement contructor needs string argument so we need to convert the key to string format.
    std::stringstream sso;
    sso << this->id;
    sso << " ";
    for(size_t i = 0; i < VectorizedCurve.size(); ++i){
        if(i != 0)
            sso << " ";
        sso << VectorizedCurve[i];
    }
    string str_curve = sso.str();

    return str_curve;

}

//Displaying each value of the curve
void CurveElement::displayVectorElementArray2D()
{
    // myLogFile << "---ELEMENT ARRAY---" << std::endl;
    for(const auto &i : arrayElementTwoD)
        myLogFile<<" (" << get<0>(i)<<","<<get<1>(i)<<"), ";

}

//The rest are debug methods
void CurveElement::displayId()
{
    // myLogFile << "id is: " << id << std::endl;
}

void CurveElement::displayVectorElementArray()
{
    // myLogFile << "---ELEMENT ARRAY---" << std::endl;
    for(const auto &i : arrayElementOneD)
        myLogFile<<i <<endl;

}

void CurveElement::displayVectorElementGrid()
{
    // myLogFile << "---ELEMENT ARRAY---" << std::endl;
    for(const auto &i : arrayElementTwoD)
        myLogFile<<get<0>(i)<<"  "<<get<1>(i) <<endl;

    //myLogFile << "size " << arrayElementTwoD.size() << endl;
}

void CurveElement::displayDistanceCurrQ()
{
    cout << distanceCurrQ << endl;
}

void CurveElement::setDistanceRandom()
{
    unsigned long x;
    x = rand();
    x <<= 15;
    x ^= rand();
    x %= 10000000;
    distanceCurrQ = x + 1;
}

void CurveElement::getL2Distance(CurveElement *q)
{
    double temp;
    double square;
    double distance = 0.0;
    for (int i = 0; i < size; i++)
    {
    //    temp = this->arrayVectorElement[i] - q->arrayVectorElement[i];
        square = pow(temp, 2.0);
        distance += square;
    }

    this->distanceCurrQ = sqrt(distance);
}

//Destructor
CurveElement::~CurveElement()
{

    if (!mark_deletion){
        delete[] QueryTrickid;
    }
}