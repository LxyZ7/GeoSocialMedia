#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <algorithm>
#include <map>
#include <time.h>
#include <string>
#include <math.h>
#include <sstream>

#define THETA 0
#define CITYFROM 0
#define pi 3.1415926535
#define FRETHR 1

using namespace std;

typedef struct localtrans{
    double out[281];
    double goback[281];
}localtrans;

typedef struct trecords{
    string id;
    int city, day, len;
    double la, lo;
    string terms[40];
}trecords;

typedef struct termdef{
    string term;
    int fre;
    int cityn;
}termdef;

double deg2rad(double deg);
double rad2deg(double rad);
double distance(double lat1, double lon1, double lat2, double lon2, char unit);
//double midselect(vector<double> dislist, int order);

vector<trecords> cityusers;

localtrans localtran[281];
// /Users/JZ/Dropbox/Research-----------------------Work/Event Detection/Experiment/NewAlgorithmonMac
int main()
{
    map<int, int> cityin;
    int cityids[50];
    FILE *citylist = fopen("..\\NewAlgorithmonMac\\citylist.txt", "r");
    for(int i = 0; i < 40; i ++){
        int cityid;
        fscanf(citylist, "%d", &cityid);
        cityin[cityid] = 1;
        cityids[i] = cityid;
    }

    FILE *cityco = fopen("..\\NewAlgorithmonMac\\coordinate.txt", "r");
    double cityla[300], citylo[300];
    for(int i = 0; i < 270; i++){
        fscanf(cityco, "%lf%lf", &cityla[i], &citylo[i]);
    }

    map<string, bool> stopword;
    FILE *sw = fopen("..\\NewAlgorithmonMac\\stopwords.txt", "r");
    char sword[30];
    while(fscanf(sw, "%s", sword) == 1){
        string swstr(sword);
        stopword[swstr] = true;
    }
    //Load language model
    map<string, double> probability[300];
    map<string, double> inprobability[300];
    map<string, double> outprobability[300];
    vector<int> tfre(300, 0);
    vector<int> ifre(300, 0);
    vector<int> ofre(300, 0);
    map<termdef, int> termfre;
    vector<termdef> termlist;
    for(int i = 0; i < 281; i++){
        if(cityin.find(i) == cityin.end())
            continue;
        vector<int> frequency;
        vector<int> infrequency;
        vector<int> outfrequency;
        vector<string> terms;
        char fname[300];
        strcpy(fname, "..\\Language Model\\CityTerms\\");
        char infname[300];
        strcpy(infname, "..\\Language Model\\InCityTerms\\");
        char outfname[300];
        strcpy(outfname, "..\\Language Model\\OutCityTerms\\");
        char forder[10];
        //itoa(i, forder, 10);
        sprintf(forder, "%d", i);
        strcat(fname, forder);
        //cout << fname << endl;
        FILE *cityterm = fopen(fname, "r");
        strcat(infname, forder);
        FILE *incityterm = fopen(infname, "r");
        strcat(outfname, forder);
        FILE *outcityterm = fopen(outfname, "r");
        if(cityterm != NULL)
            cout << "Opening:" << i << endl;
        else
            cout << "Cannot open the file!" << endl;

        char term[100];
        int freq;
        int localtotal = 0;
        while(fscanf(cityterm, "%s%d", term, &freq) == 2){
            string interm(term);
            if(interm.length() == 0) continue;
            frequency.push_back(freq);
            terms.push_back(interm);
            localtotal += freq;

        }
        cout << localtotal << endl;
        tfre[i] = localtotal;
        for(int j = 0; j < terms.size(); j++){
            if(frequency[j] > FRETHR)
            probability[i][terms[j]] = 110.0 * (double)(frequency[j] + THETA) / localtotal;
        }

        //do a investigation on the influence of frequency of terms
        /*
        if(i == 0){
            sort(frequency.begin(), frequency.end());
            int cumm = 0, times = 1;  //353308
            for(int j = 0; j < frequency.size(); j++){
                cumm += frequency[j];
                if(cumm > times * 353308){
                    cout << j << '\t' << frequency[j] << endl;
                    times++;
                }
            }
        }
        return 0;
*/
        //finish

        terms.clear();
        localtotal = 0;
        while(fscanf(incityterm, "%s%d", term, &freq) == 2){
            string interm(term);
            if(interm.length() == 0) continue;
            infrequency.push_back(freq);
            terms.push_back(interm);
            localtotal += freq;
        }
        cout << localtotal << endl;
        ifre[i] = localtotal;
        for(int j = 0; j < terms.size(); j++){
            if(infrequency[j] > FRETHR)
            inprobability[i][terms[j]] = 100.0 * (double)(infrequency[j] + THETA) / localtotal;
        }

        terms.clear();
        localtotal = 0;
        while(fscanf(outcityterm, "%s%d", term, &freq) == 2){
            string interm(term);
            if(interm.length() == 0) continue;
            outfrequency.push_back(freq);
            terms.push_back(interm);
            localtotal += freq;
        }
        cout << localtotal << endl;
        ofre[i] = localtotal;
        for(int j = 0; j < terms.size(); j++){
            if(outfrequency[j] > FRETHR)
            outprobability[i][terms[j]] = 100.0 * (double)(outfrequency[j] + THETA) / localtotal;
        }
    }

    //Load transition model
    int transfer[300][300];
    FILE *trans = fopen("..\\NewAlgorithmonMac\\transfermatrix.txt", "r");
    for(int i = 0; i < 281; i++){
        for(int j = 0; j < 281; j++){
            fscanf(trans, "%d\t", &transfer[i][j]);
        }
    }

    double comtrans[300][300];
    for(int i = 0; i < 281; i++){
        if(cityin.find(i) == cityin.end()) continue;
        int linecount = 0;

        for(int j = 0; j < 281; j++){
            linecount += transfer[i][j];
        }

        for(int j = 0; j < 281; j++){
            comtrans[i][j] = (double)transfer[i][j] / linecount;
            if(transfer[i][j] == 0)
                comtrans[i][j] = 1 / linecount;
        }
    }

    for(int i = 0; i < 281; i++){
        if(cityin.find(i) == cityin.end()) continue;
        for(int j = 0; j < 281; j++){
            if(cityin.find(j) == cityin.end()) continue;
            cout << comtrans[i][j] << '\t';
        }
        cout << endl;
    }

    for(int i = 0; i < 281; i++){
        if(cityin.find(i) == cityin.end())
            continue;
        vector<int> frequency;
        vector<string> terms;
        char fname[300];
        strcpy(fname, "..\\TransProbability\\CityTransfer\\");
        char forder[10];
        //itoa(i, forder, 10);
        sprintf(forder, "%d", i);
        strcat(fname, forder);
        FILE *cityterm = fopen(fname, "r");
        if(cityterm != NULL)
            cout << "Opening:" << i << endl;
        else
            cout << "Cannot open the file!" << endl;
        for(int j = 0; j < 281; j++){
            fscanf(cityterm, "%lf%lf", &localtran[i].out[j], &localtran[i].goback[j]);
        }
    }

    //Inference
    int A1a = 0, A1total = 0, A2a = 0, A2total = 0, A3a = 0, A4a = 0;
    int outtotal = 0, A1o = 0, A2o = 0, A3o = 0, A4o = 0;
    int intotal = 0, A1i = 0, A2i = 0, A3i = 0, A4i = 0;
    vector<double> A1list, A2list, A3list, A4list;
    vector<double> oA1list, oA2list, oA3list, oA4list;
    double A1td = 0, A2td = 0, A3td = 0, A4td = 0;
    int A1c = 0, A2c = 0, A3c = 0, A4c = 0;
    double oA1td = 0, oA2td = 0, oA3td = 0, oA4td = 0;
    int oA1c = 0, oA2c = 0, oA3c = 0, oA4c = 0;
    for(int i = CITYFROM; i < 281; i++){
        if(cityin.find(i) == cityin.end())
            continue;
        cityusers.clear();
        char fname[300];
        strcpy(fname, "..\\UserAssign\\citylist\\");
        char forder[10];
        //itoa(i, forder, 10);
        sprintf(forder, "%d", i);
        strcat(fname, forder);
        FILE *cityterm = fopen(fname, "r");
        if(cityterm != NULL)
            cout << "Opening:" << i << endl;
        else
            cout << "Cannot open the file!" << endl;

        char l1[200], l2[500];
        while(fgets(l1, 140, cityterm)){
            char uid[20];
            int city, day;
            char temp1[40], temp2[40], temp3[40];
            double tla, tlo;
            sscanf(l1, "%s%d%d%s%s%s%lf%lf", uid, &city, &day, temp1, temp2, temp3, &tla, &tlo);
            //cout << A2total << endl;

            fgets(l2, 240, cityterm);
            if(cityin.find(city) == cityin.end()) continue;
            string userid(uid);
            trecords onerecord;
            onerecord.id = userid;
            onerecord.day = day;
            onerecord.city = city;
            onerecord.la = tla;
            onerecord.lo = tlo;

            for(int j = 0; l2[j] != '\0'; j++){
                if(l2[j] == '.' || l2[j] == ',' || (l2[j] == ':' && (l2[j-1] != 's' && l2[j-1] != 'p')) || l2[j] == '^' || l2[j] == '`' || l2[j] == ';' || l2[j] == '~' || l2[j] == '?' || l2[j] == '!' || l2[j] == '*' || l2[j] == '|' || l2[j] == '"' || l2[j] == '=' || l2[j] == '(' || l2[j] == ')' || l2[j] == '[' || l2[j] == ']' || l2[j] == '-' || l2[j] == '+' || l2[j] == '*' || l2[j] == '$'){
                    l2[j] = ' ';
                }
                if(l2[j] == '\\' && l2[j+1] == '"'){
                    l2[j] = ' ';
                    l2[j+1] = ' ';
                }
            }
            string stringin(l2);
            istringstream line(stringin);
            int k = 0;
            while(line){
                string sub;
                line >> sub;
                bool termjudge = true;
                for(int j = 0; j < sub.length(); j++){
                    if((sub[j] >= 'a' && sub[j] <= 'z') || (sub[j] >= '0' && sub[j] <= '9') || (sub[j] == '@') || (sub[j] == '#') || sub[j] == '\'' || sub[j] == '_' || sub[j] == '%' || sub[j] == '&')
                        NULL;
                    else{
                        termjudge = false;
                    }
                }
                if(termjudge && k < 40){
                    onerecord.terms[k] = sub;
                    k++;
                }
            }
            onerecord.len = k;
            cityusers.push_back(onerecord);
            if(cityusers.size() > 200000) break;
        }
        cout << "finish reading" << endl;
        for(int j = 0; j < cityusers.size(); j++){
            int temp = j + 1;
            while(cityusers[temp].id == cityusers[j].id && temp < cityusers.size()){
                temp++;
            }
            if(temp - j >= 10 && temp - j < 500){
                int lastcity = i;
                int startday = cityusers[j].day;
                vector<int> citypath;
                vector<double> tcityla;
                vector<double> tcitylo;
                vector<double> pathpro(300, 0);
                vector< vector<int> > pathrecord;
                for(; j < temp; j++){   //scan tweets of the same user
                    vector<double> A1(300, 0);
                    vector<double> A2(300, 0);
                    vector<double> A3(300, 0);
                    citypath.push_back(cityusers[j].city);
                    tcityla.push_back(cityusers[j].la);
                    tcitylo.push_back(cityusers[j].lo);

                    double stotal = cityusers[j].len;
                    vector<int> scount(40, 0);
                    for(int m = 0; m < cityusers[j].len; m++){
                        int tcount = 1;
                        if(scount[m] == -1) {
                            scount[m] = 0;
                            continue;
                        }
                        for(int n = m + 1; n < cityusers[j].len; n++){
                            if(cityusers[j].terms[m] == cityusers[j].terms[n]){
                                tcount ++;
                                scount[n] = -1;
                            }
                        }
                        scount[m] = tcount;
                    }

                    for(int k = 0; k < 281; k++){  //scan city
                        if(cityin.find(k) == cityin.end()) continue;

                        for(int m = 0; m < cityusers[j].len; m++){  //scan terms
                            double prob;
                            if(stopword.find(cityusers[j].terms[m]) != stopword.end()) continue;
                            if(probability[k].find(cityusers[j].terms[m]) != probability[k].end())
                                prob = probability[k][cityusers[j].terms[m]];// + 10000.0 / tfre[k];
                            else
                                prob = 0.00000001; //100.0 / tfre[k];
                            //if(prob < 0.000001) prob = 0.000001;
                            if(scount[m] == 0) continue;
                            double lmt = (double)scount[m] / stotal;
                            A3[k] += lmt * log10(lmt / prob);
                        }

                        for(int m = 0; m < cityusers[j].len; m++){  //scan terms
                            double prob;
                            if(stopword.find(cityusers[j].terms[m]) != stopword.end()) continue;
                            if(probability[k].find(cityusers[j].terms[m]) != probability[k].end())
                                prob = probability[k][cityusers[j].terms[m]]; // + 10000.0 / tfre[k];
                            else
                                prob = 0.00000001; //100.0 / tfre[k];
                            //if(prob < 0.000001) prob = 0.000001;
                            A2[k] += log10(prob);
                        }

                        for(int m = 0; m < cityusers[j].len; m++){  //scan terms
                            double prob;
                            if(stopword.find(cityusers[j].terms[m]) != stopword.end()) continue;
                            if(k == i){
                                if(inprobability[k].find(cityusers[j].terms[m]) != inprobability[k].end())
                                    prob = inprobability[k][cityusers[j].terms[m]]; // + 10000.0 / ifre[k];
                                else
                                    prob = 0.00000001; //100.0 / ifre[k];
                                //if(prob < 0.000001) prob = 0.000001;
                                A1[k] += log10(prob);
                            }
                            else{
                                if(outprobability[k].find(cityusers[j].terms[m]) != outprobability[k].end())
                                    prob = outprobability[k][cityusers[j].terms[m]]; // + 10000.0 / ofre[k];
                                else
                                    prob = 0.00000002; //100.0 / ofre[k]; //10000.0 / ofre[k];
                                //if(prob < 0.000001) prob = 0.000001;
                                A1[k] += log10(prob);
                            }

                        }
                        //A1[k] = A2[k];

                    }
                    int max1 = 0, max2 = 0, max3 = 0;

                    vector<double> newhmm(300, 0);
                    if(pathrecord.size() == 0){
                        vector<int> cstate(300, i);
                        pathrecord.push_back(cstate);
                        for(int k = 0; k < 281; k++){
                            if(cityin.find(k) == cityin.end()) continue;
                            newhmm[k] = log10(localtran[i].out[k] + 0.000000001) + A1[k];
                        }
                    }
                    else{
                        vector<int> cstate(300, 0);
                        for(int k = 0; k < 281; k++){
                            if(cityin.find(k) == cityin.end()) continue;
                            vector<double> hmmpro(300, 0);
                            double maxpro = -10000000;
                            int maxcity = 0;
                            for(int m  = 0; m < 281; m++){
                                if(cityin.find(m) == cityin.end()) continue;
                                double mpro = 0;
                                if(m == i){
                                    mpro = log10(localtran[i].out[k] + 0.000000001) + pathpro[m];
                                }
                                else{
                                    if(k == i){
                                        mpro = log10(localtran[i].goback[m] + 0.000000001) + pathpro[m];
                                    }
                                    else{
                                        mpro = log10(comtrans[m][k] + 0.000000001) + pathpro[m];
                                        if(cityusers[j].day - cityusers[j-1].day > 7 ){ //&& m == k){
                                            //mpro = log10(0.8) + pathpro[m];
                                            mpro = log10(localtran[i].out[k] + 0.000000001) + pathpro[m];
                                        }
                                    }
                                }
                                if(mpro > maxpro){
                                    maxcity = m;
                                    maxpro = mpro;
                                }
                            }
                            newhmm[k] = maxpro + A1[k];
                            cstate[k] = maxcity;
                        }
                        pathrecord.push_back(cstate);
                    }

                    for(int k = 0; k < 281; k++){
                        if(cityin.find(k) == cityin.end()) continue;
                        pathpro[k] = newhmm[k];
                    }

                    for(int k = 1; k < 281; k++){
                        if(cityin.find(k) == cityin.end()) continue;
                        //if(A1[k] > A1[max1]) max1 = k;
                        if(A2[k] > A2[max2]) max2 = k;
                        if(A3[k] < A3[max3]) max3 = k;
                    }
                    lastcity = max1;
                    //A1total++;
                    A2total++;
                    if(cityusers[j].city != i)
                        outtotal++;
                    else
                        intotal++;
                    /*
                     if(max1 == cityusers[j].city) {
                     A1a++;
                     if(cityusers[j].city != i)
                     A1o++;
                     else
                     A1i++;
                     }*/
                    double errordis = distance(cityusers[j].la, cityusers[j].lo, cityla[max2], citylo[max2], 'K');
                    A2td += errordis;
                    A2c ++;
                    A2list.push_back(errordis);
                    if(cityusers[j].city != i){
                        oA2td += errordis;
                        oA2c ++;
                        oA2list.push_back(errordis);
                    }

                    if(max2 == cityusers[j].city){
                        A2a++;
                        if(cityusers[j].city != i)
                            A2o++;
                        else
                            A2i++;
                    }
                    errordis = distance(cityusers[j].la, cityusers[j].lo, cityla[max3], citylo[max3], 'K');
                    A3td += errordis;
                    A3c ++;
                    A3list.push_back(errordis);
                    if(cityusers[j].city != i){
                        oA3td += errordis;
                        oA3c ++;
                        oA3list.push_back(errordis);
                    }

                    if(max3 == cityusers[j].city){
                        A3a++;

                        if(cityusers[j].city != i)
                            A3o++;
                        else
                            A3i++;
                    }
                    srand((int)time(0));
                    int randomcity = rand() % 40;
                    errordis = distance(cityusers[j].la, cityusers[j].lo, cityla[cityids[randomcity]], citylo[cityids[randomcity]], 'K');
                    A4td += errordis;
                    A4c ++;
                    A4list.push_back(errordis);
                    if(cityusers[j].city != i){
                        oA4td += errordis;
                        oA4c ++;
                        oA4list.push_back(errordis);
                    }

                    if(cityids[randomcity] == cityusers[j].city){
                        A4a++;

                        if(cityusers[j].city != i)
                            A4o++;
                        else
                            A4i++;
                    }

                    if(A2total % 10000 == 0){
                        cout << double(A1a) / A2total << '\t' << double(A2a) / A2total << '\t' << double(A3a) / A2total << '\t' << double(A4a) / A2total <<  '\n' << double(A1o) / outtotal << '\t' << double(A2o) / outtotal << '\t' << double(A3o) / outtotal << '\t' << double(A4o) / outtotal << '\n' << double(A1i) / intotal << '\t' << double(A2i) / intotal << '\t' << double(A3i) / intotal << '\t' << double(A4i) / intotal << endl;
                        cout << "Error Distance:" << endl;
                        sort(A1list.begin(), A1list.end());
                        sort(A2list.begin(), A2list.end());
                        sort(A3list.begin(), A3list.end());
                        sort(A4list.begin(), A4list.end());
                        sort(oA1list.begin(), oA1list.end());
                        sort(oA2list.begin(), oA2list.end());
                        sort(oA3list.begin(), oA3list.end());
                        sort(oA4list.begin(), oA4list.end());
                        int middle = A2total / 2;
                        cout << A1list[A1list.size()/2] << '\t' <<  A2list[A2list.size()/2] << '\t' <<  A3list[A3list.size()/2] << '\t' <<  A4list[A4list.size()/2] << '\t' << endl;
                        cout << A1td/A1c << '\t' <<  A2td/A2c << '\t' <<  A3td/A3c << '\t' <<  A4td/A4c << '\t' << endl;
                        cout << oA1list[oA1list.size()/2] << '\t' <<  oA2list[oA2list.size()/2] << '\t' <<  oA3list[oA3list.size()/2] << '\t' <<  oA4list[oA4list.size()/2] << '\t' << endl;
                        cout << oA1td/oA1c << '\t' <<  oA2td/oA2c << '\t' <<  oA3td/oA3c << '\t' <<  oA4td/A4c << '\t' << endl;
                        cout << A1total << '\t' << outtotal << '\t' << intotal << endl;
                        //cout << A1a << '\t' << A1total << endl;
                    }
                }
                A1total += (temp - j);
                //Statistics on the accuracy of algorithm 1
                int maxpath = 0;
                for(int k = 1; k < 281; k++){
                    if(cityin.find(k) == cityin.end()) continue;
                    if(pathpro[k] > pathpro[maxpath])
                        maxpath = k;
                }
                for(int k = citypath.size(); k > 0; k--){
                    double errordis = distance(tcityla[k-1], tcitylo[k-1], cityla[maxpath], citylo[maxpath], 'K');
                    A1td += errordis;
                    A1c ++;
                    A1list.push_back(errordis);
                    if(citypath[k-1] != i){
                        oA1td += errordis;
                        oA1c ++;
                        oA1list.push_back(errordis);
                    }
                    if(maxpath == citypath[k-1]){
                        A1a++;
                        if(citypath[k-1] != i)
                            A1o++;
                        else
                            A1i++;
                    }
                    maxpath = pathrecord[k - 1][maxpath];
                }
            }
        }
    }
    cout << "Final results: " << double(A1a) / A1total << '\t' << double(A2a) / A2total;
    return 0;
}




double deg2rad(double deg) {
    return (deg * pi / 180);
}

double rad2deg(double rad) {
    return (rad * 180 / pi);
}

double distance(double lat1, double lon1, double lat2, double lon2, char unit) {
    double theta, dist;
    theta = lon1 - lon2;
    dist = sin(deg2rad(lat1)) * sin(deg2rad(lat2)) + cos(deg2rad(lat1)) * cos(deg2rad(lat2)) * cos(deg2rad(theta));
    dist = acos(dist);
    dist = rad2deg(dist);
    dist = dist * 60 * 1.1515;
    switch(unit) {
        case 'M':
            break;
        case 'K':
            dist = dist * 1.609344;
            break;
        case 'N':
            dist = dist * 0.8684;
            break;
    }
    return (dist);
}













