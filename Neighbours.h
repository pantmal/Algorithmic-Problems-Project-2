#ifndef NEIGHBOURS_H
#define NEIGHBOURS_H


#include <string>

//neighboursInfo class
class neighboursInfo
{
public:
    static int counterId;
    static int counterDistance;
    std::string *arrayId;
    double *arrayDistance;
    void passTheValues(int k)
    {
        for (int i = 0; i < k; i++)
        {

            this->arrayId[i] = counterId++;
            this->arrayDistance[i] = counterDistance++;
        }
    }
    neighboursInfo(int N);
    ~neighboursInfo();
};

#endif //NEIGHBOURS_H