#include <cmath>
#include <bits/stdc++.h>

#include "CurveElement.h"
#include "Helpers.h"

using namespace std;

//std::ofstream myLogFile;

CurveElement::CurveElement(int dimensions, std::string fileLine, int numberOfHashTables, std::string variant)
{

    assigned = false;

    QueryTrickid = new unsigned int[numberOfHashTables]; //gets set when the vector element gets into an LSH bucket
    
    //arrayElementTwoD.clear();
    //arrayElementOneD.clear();

    id = "0";
    double temp;
    unsigned int counter = 0;
    distanceCurrQ = 0;
    
    size = dimensions;
   // arrayVectorElement = new double[size];
    
    std::stringstream sso(fileLine);
    sso >> id; //Get the id
    while (sso >> temp) //And the vector values in the array field
    {
        // arrayVectorElement[counter] = temp;
        if (variant == "discrete"){
            arrayElementTwoD.push_back({counter+1,temp});
            gridElementTwoD.push_back({0,0.0});
        }else{
            arrayElementOneD.push_back(temp);
            //grid?
            filteredElementOneD.push_back(0.0);
        }    
        counter++;
    }
    

}

CurveElement::CurveElement(std::vector<std::tuple <unsigned int, double, unsigned int, double > >& traversal){


    for (const auto &i : traversal){
        unsigned int Px = get<0>(i);
        double Py = get<1>(i);
        unsigned int Qx = get<2>(i);
        double Qy = get<3>(i);

        unsigned int mean_x = (Px + Qx) / 2;
        double mean_y = (Py + Qy) / 2;

        arrayElementTwoD.push_back({mean_x,mean_y});
    }


}

void CurveElement::Snapping2d(double t1, double t2, double delta, int columns){

    gridElementTwoD.clear();

    int prev_x = 0;
    double prev_y = 0;
    for (int j = 0; j < columns; j++){
        
        //x = floor((x-t)/d + 1/2) * d + t
        int x = get<0>( arrayElementTwoD[j]);
        double y = get<1>( arrayElementTwoD[j]);
        
        int grid_x = (floor((abs(x-t1)/delta) + 0.5) * delta) + t1;
        double grid_y = (floor((abs(y-t2)/delta) + 0.5) * delta) + t2;
        
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

string CurveElement::Vectorization2d(int columns, int M){


    vector<double>  VectorizedCurve;
    int grid_size = gridElementTwoD.size();
    for (int v = 0; v < grid_size ;v++ ){
        int x = get<0>( gridElementTwoD[v]);
        double y = get<1>( gridElementTwoD[v]);

        VectorizedCurve.push_back(x);
        VectorizedCurve.push_back(y);
        
    }

    int size_before_pad = VectorizedCurve.size();
    
    for (int pad_c = size_before_pad; pad_c < 2*columns; pad_c++){
        VectorizedCurve.push_back(M);
    }
    
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

void CurveElement::Filtering(double e){

    
    filteredElementOneD.clear(); //TODO: MAYBE DEL? ALSO ADD IT FOR GRIDS
    vector<double> v = arrayElementOneD;
    
    auto it = v.begin();
    while (it != v.end()){
        
        if (it >= v.end() || it+1 >= v.end() || it+2 >= v.end()){
            break;
        }

        double a = *it;
        double b = *(it+1);
        double c = *(it+2);

        // myLogFile << "a b c "<< a << " " << b << " "<< c << endl;

        double res1 = abs(a-b);
        double res2 = abs(b-c);

        //it = it+3;
        if (res1 <= e && res2 <= e){
            v.erase(it+1);
            //it = it+1;
        }else{
            it = it+1;
        }

    }

    filteredElementOneD = v;


}

void CurveElement::Snapping1d(double delta){

    int filt_size = filteredElementOneD.size(); 
    for (int j = 0; j < filt_size; j++ ){

        double y = filteredElementOneD[j];
        double grid_y = floor(abs(y)/delta) * delta;   

        gridElementOneD.push_back(grid_y);
    }
}

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

        //it = it+3;
        if (min_val <= b && max_val >= b){
            v2.erase(it2+1);
            //it = it+1;
        }else{
            it2 = it2+1;
        }


    }
    
    gridElementOneD = v2;


}

string CurveElement::Vectorization1d(int columns, int M){


    vector<double>  VectorizedCurve;
    int grid_size = gridElementOneD.size();
    for (int v = 0; v < grid_size ;v++ ){
        
        double y = gridElementOneD[v];
        VectorizedCurve.push_back(y);
    }

    int size_before_pad = VectorizedCurve.size();
    for (int pad_c = size_before_pad; pad_c < columns; pad_c++){
        VectorizedCurve.push_back(M);
    }
    
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

//Debug method
void CurveElement::displayId()
{
    // myLogFile << "id is: " << id << std::endl;
}

//Displaying each column/value of the vector
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

    myLogFile << "size " << arrayElementTwoD.size() << endl;
}



//Debug method
void CurveElement::displayDistanceCurrQ()
{
    cout << distanceCurrQ << endl;
}

//Debug method
void CurveElement::setDistanceRandom()
{
    unsigned long x;
    x = rand();
    x <<= 15;
    x ^= rand();
    x %= 10000000;
    distanceCurrQ = x + 1;
}

//Getting the L2 distance with the q/argument specified and setting it to the field.
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

   // delete[] arrayVectorElement;
    delete[] QueryTrickid;
}