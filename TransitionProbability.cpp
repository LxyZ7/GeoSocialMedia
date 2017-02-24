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

using namespace std;

int main()
{
    map<int, int> cityin;
    FILE *citylist = fopen("citylist.txt", "r");
    for(int i = 0; i < 40; i ++){
        int cityid;
        fscanf(citylist, "%d", &cityid);
        cityin[cityid] = 1;
    }

    //Language Model
    for(int i = 0; i < 281; i++){
        if(cityin.find(i) == cityin.end())
            continue;
        char fname[100];
        strcpy(fname, "..\\UserAssign\\citytweets\\");
        char forder[10];
        itoa(i, forder, 10);
        strcat(fname, forder);
        FILE *cityterm = fopen(fname, "r");
        if(cityterm != NULL)
            cout << "Opening:" << i << endl;
        else
            cout << "Cannot open the file!" << endl;

        strcpy(fname, "CityTerms\\");
        strcat(fname, forder);
        FILE *citycount = fopen(fname, "w");

        strcpy(fname, "OutCityTerms\\");
        strcat(fname, forder);
        FILE *outcitycount = fopen(fname, "w");
        strcpy(fname, "InCityTerms\\");
        strcat(fname, forder);
        FILE *incitycount = fopen(fname, "w");

        if(citycount != NULL)
            cout << "Writing:" << i << endl;
        else
            cout << "Cannot open the file!" << endl;

        char l1[200], l2[500];
        vector<string> terms;
        map<string, int> termcount;

        vector<string> termsin;
        map<string, int> termcountin;

        vector<string> termsout;
        map<string, int> termcountout;

        while(fgets(l1, 140, cityterm)){
            char luid[20];
            int cityid;
            sscanf(l1, "%s%d", luid, &cityid);
            if(cityid != i) cout << "City: " << i << endl;
            fgets(l2, 240, cityterm);
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
            while(line){
                string sub;
                line >> sub;
                bool termjudge = true;
                for(int j = 0; j < sub.length(); j++){
                    if((sub[j] >= 'a' && sub[j] <= 'z') || (sub[j] >= '0' && sub[j] <= '9') || (sub[j] == '@') || (sub[j] == '#') || sub[j] == '\'' || sub[j] == '_' || sub[j] == '%' || sub[j] == '&')
                        NULL;
                    else{
                        //if(sub[j] != '\\' && sub[j] != '/')
                            //cout << sub <<endl;
                        termjudge = false;
                    }
                }
                if(termjudge){
                    if(termcount.find(sub) != termcount.end()){
                        termcount[sub] ++;
                    }
                    else{
                        termcount[sub] = 1;
                        terms.push_back(sub);
                    }
                }
                if(termjudge && cityid == i){
                    if(termcountin.find(sub) != termcountin.end()){
                        termcountin[sub] ++;
                    }
                    else{
                        termcountin[sub] = 1;
                        termsin.push_back(sub);
                    }
                }
                if(termjudge && cityid != i){
                    if(termcountout.find(sub) != termcountout.end()){
                        termcountout[sub] ++;
                    }
                    else{
                        termcountout[sub] = 1;
                        termsout.push_back(sub);
                    }
                }
            }
        }

        for(int j = 0; j < terms.size(); j++){
            if(terms[j].length() > 0 && (terms[j].length() != 1 || terms[j][0] != '@'))
            fprintf(citycount, "%s\t%d\n", terms[j].c_str(), termcount[terms[j]]);
        }

        for(int j = 0; j < termsin.size(); j++){
            if(termsin[j].length() > 0 && (termsin[j].length() != 1 || termsin[j][0] != '@'))
            fprintf(incitycount, "%s\t%d\n", termsin[j].c_str(), termcountin[termsin[j]]);
        }
        for(int j = 0; j < termsout.size(); j++){
            if(termsout[j].length() > 0 && (termsout[j].length() != 1 || termsout[j][0] != '@'))
            fprintf(outcitycount, "%s\t%d\n", termsout[j].c_str(), termcountout[terms[j]]);
        }
    }

    return 0;
}













