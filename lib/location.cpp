#include "friendbased.h"

void Naive_Location_Prediction(int begin, int end){
    double Probability[10000], TempProbability, Temp;
    int ProHelp[10000], Distance;
    int i, j, record = begin;
    int centercount = 0;
    for(i = 0; i < 10000; i++){
        ProHelp[i] = 0;
        Probability[i] = 1;
    }
    for(i = begin; i != end + 1; i++){
        for(j = begin; j != end + 1; j++){
            if(j == i)
                continue;
            Distance = DistanceCalculator(LazyLatitude[i], LazyLongtitude[i], LazyLatitude[j], LazyLongtitude[j]) / 10;
            Temp = (double)Distance + 0.196;
            TempProbability = 0.0019 * pow(Temp, -1.05);
            while(TempProbability < 0.1){
                TempProbability = TempProbability * 10;
                ProHelp[i]++;
            }
            Probability[i] = Probability[i] * TempProbability;
            while(Probability[i] < 0.1){
                Probability[i] = Probability[i] * 10;
                ProHelp[i]++;
            }
        }
    }
    for(i = begin+1; i != end + 1; i++){
        if((Probability[i] > Probability[record] && ProHelp[i] == ProHelp[record]) || ProHelp[i] < ProHelp[record])
            record = i;
    }
    UserLocationLatitude = LazyLatitude[record];
    UserLocationLongtitude = LazyLongtitude[record];
    //Add a new part
    int La = 0, Lo = 0, num = 0;
    for(i = begin; i <= end; i++){
        if(DistanceCalculator(LazyLatitude[i], LazyLongtitude[i], UserLocationLatitude, UserLocationLongtitude) < 500){
            centercount ++;
        }
    }
    PredictionHelp = 1;
    //if(centercount >= ((end-begin+1)/3) && (end - begin + 1) > 0)
        PredictionHelp = 1;
}
