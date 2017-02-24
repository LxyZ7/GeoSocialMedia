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


#define pi 3.1415926535

using namespace std;

typedef struct wordlist{
    string word;
    int fre;
    int dis[80];
    double ave;
    double cc;
}wordlist;

bool sorthelp(wordlist a, wordlist b){
    return a.fre > b.fre;
}

typedef struct tweetlist{
    char tweet[300];
    double weight;
}tweetlist;

bool ktsorthelp(tweetlist a, tweetlist b){
    return a.weight > b.weight;
}

double deg2rad(double deg) {
  return (deg * pi / 180);
}

double rad2deg(double rad) {
  return (rad * 180 / pi);
}
double distance(double lat1, double lon1, double lat2, double lon2);

vector<wordlist> words;
vector<double> eventtime_;
double avgt;
int main()
{
    FILE *in = fopen("..\\UserAssign\\citytweets\\1", "r");
    if(in == NULL) cout << "FAIL TO OPEN!" << endl;

    map<string, int> wordcount;
    vector<string> lists;

    int totaldis[100];
    for(int i = 0; i < 100; i ++) totaldis[i] = 0;
    double totalave = 0;
    vector<int> hourlycount(5000, 0);
    vector<int> daycount(1000, 0);

    char l1[200], l2[20], l3[500];
    int lowerhour = 5000, upperhour = 0;
    while(fgets(l1, 140, in) && 0){
        fgets(l2, 10, in);
        fgets(l3, 260, in);
        char id[30], month[20], time[20];
        int city, days, date;
        double la, lo;
        sscanf(l1, "%s%d%d%s%d%s%lf%lf", id, &city, &days, month, &date, time, &la, &lo);
        int staherehour;
        sscanf(time, "%d", &staherehour);
        int currhour = days*24+staherehour;
        hourlycount[currhour]++;
        daycount[days]++;
        lowerhour = lowerhour < currhour ? lowerhour : currhour;
        upperhour = upperhour > currhour ? upperhour : currhour;

        int k = 0;
        char wordcut[200];
        for(int i = 0; l3[i] != '\0'; i++){
            bool fw = true;
            if((l3[i] == ' ' || l3[i] == '"')){
                if(fw && (k > 0 || k < 15) && wordcut[0] != ' '){
                    wordcut[k] = '\0';
                    string nword = wordcut;
                    if(wordcount.find(nword) != wordcount.end()){
                        int worder = wordcount[nword];
                        words[worder].fre++;
                        words[worder].dis[days]++;
                        totaldis[days]++;
                    }
                    else{
                        wordcount[nword] = words.size();
                        lists.push_back(nword);
                        wordlist newword;
                        newword.word = nword;
                        newword.fre = 1;
                        for(int j = 0; j < 80; j++){ newword.dis[j] = 0;}
                        newword.ave = 0;
                        words.push_back(newword);
                    }
                }
                k = 0;
            }
            else if((l3[i] >= 'a' && l3[i] <= 'z') || (l3[i] >= '0' && l3[i] <= '9') || l3[i] == '@' || l3[i] == '#' || l3[i] == '\''){
                wordcut[k] = l3[i];
                k++;
            }
            else{
                fw = false;
                k++;
            }
        }
    }
    /*
    FILE *frecout = fopen("frequencyofteeetsoverall_day.txt", "w");
    for(int i = 32; i <= 61; i++)
        fprintf(frecout, "%d\t", daycount[i]);
    return 0;
    vector<int> maxhour(26, 0);
    for(int i = lowerhour; i <= upperhour; i++)
        maxhour[i%24] = maxhour[i%24] > hourlycount[i] ? maxhour[i%24] : hourlycount[i];
    for(int i = 0; i < 24; i++)
        fprintf(frecout, "%d\t", maxhour[i]);
    cout << lowerhour << '\t' << upperhour << endl;
    return 0;
*/
    for(int  i = 24; i < 66; i++){
        totalave += totaldis[i];
    }
    totalave = totalave / 42;

    for(int i = 0; i < lists.size(); i++){
        int n = wordcount[lists[i]];
        for(int j = 24; j < 66; j++){
            words[n].ave += words[n].dis[j];
        }
        words[n].ave = words[n].ave / 42;
        double upper = 0, down1 = 0, down2 = 0;
        for(int j = 24; j < 66; j++){
            upper += (words[n].dis[j] - words[n].ave) * (totaldis[j] - totalave);
            down1 += (words[n].dis[j] - words[n].ave) * (words[n].dis[j] - words[n].ave);
            down2 += (totaldis[j] - totalave) * (totaldis[j] - totalave);
        }
        words[n].cc = upper / (sqrt(down1) * sqrt(down2));
    }

/*
    for(int i = 0; i < lists.size(); i++){
        wordlist newword;
        newword.word = lists[i];
        newword.fre = wordcount[lists[i]];
        words.push_back(newword);
    }
*/
    sort(words.begin(), words.end(), sorthelp);

    FILE *out = fopen("wordlist.txt", "w");
    FILE *out1 = fopen("wordlist1.txt", "w");
    int eventorder = 0;
    long long totalfre = 0;
    int numofwords = 0;
    map<string, int> daylist[100];
    for(int i = 0; i < words.size(); i++){
        if(words[i].fre < 20) continue;
        fprintf(out, "%d\t%s\t%d\t%lf\n", i, words[i].word.c_str(), words[i].fre, words[i].cc);
        totalfre += words[i].fre;
        numofwords++;
        if(words[i].fre > 30 && words[i].cc < 0.35){
            fprintf(out1, "%s\t", words[i].word.c_str());
            int freh = 0;
            for(int j = 25; j < 66; j++){
                fprintf(out1, "%d\t", words[i].dis[j]);
                freh += words[i].dis[j];
            }
            fprintf(out1, "\n");
            for(int j = 25; j < 66; j++){
                if(words[i].dis[j] > 0.3 * freh){
                    daylist[j][words[i].word] = eventorder;
                    eventorder++;
                }
            }
        }
    }
    cout << totalfre << '\t' << numofwords << endl;
    cout << eventorder << endl;
    //return 0;
/*
    in = fopen("..\\UserAssign\\citytweets\\1", "r");
    while(fgets(l1, 140, in)){
        fgets(l2, 10, in);
        fgets(l3, 260, in);
        char id[30], month[20], time[20];
        int city, days, date;
        double la, lo;
        sscanf(l1, "%s%d%d%s%d%s%lf%lf", id, &city, &days, month, &date, time, &la, &lo);
        int k = 0;
        char wordcut[200];
        for(int i = 0; l3[i] != '\0'; i++){
            bool fw = true;
            if((l3[i] == ' ' || l3[i] == '"')){
                if(fw && (k > 0 || k < 15) && wordcut[0] != ' '){
                    wordcut[k] = '\0';
                    string nword = wordcut;
                    if(daylist[days].find(nword) != daylist[days].end()){
                        int fid = daylist[days][nword];
                        char fname[300];
                        strcpy(fname, "Event\\");
                        char forder[10];
                        sprintf(forder, "%d", fid);
                        strcat(fname, forder);
                        FILE *eventfile = fopen(fname, "at+");
                        fprintf(eventfile, "%s\t%d\t%s\t%lf\t%lf\n%s", month, date, time, la, lo, l3);
                        fclose(eventfile);
                    }
                }
                k = 0;
            }
            else if((l3[i] >= 'a' && l3[i] <= 'z') || (l3[i] >= '0' && l3[i] <= '9') || l3[i] == '@' || l3[i] == '#' || l3[i] == '\''){
                wordcut[k] = l3[i];
                k++;
            }
            else{
                fw = false;
                k++;
            }
        }
    }

    return 0;
*/
    fclose(in);

    FILE *varr = fopen("variance.txt", "w");
    vector<double> eventtimevar, eventla, eventlo, eventdisvar;
    vector<int> eventlengthsta(20, 0);
    //vector<int> event
    //vector<int> eventdate;
    vector<bool> combined(5000, false);
    vector<int> combinedto(5000, -1);
    vector<int> combinecount(5000, 0);
    vector<int> eventlength(5000, 0);
    vector<int> eventtweets(5000, 0);
    vector<int> eventdate_;
        vector<string> eventtime;
        vector<string> eventmonth;
        //vector<tweetlist> keytweet;

    int eventcount = 0;
    for(int fid = 0; fid < 2288; fid++){
        int tweetscount = 0;
        map<string, int>termcount;
        map<string, int>termoccur;
        vector<string> termlist;
        char fname[300];
        strcpy(fname, "Event\\");
        char forder[10];
        sprintf(forder, "%d", fid);
        strcat(fname, forder);
        FILE *eventfile = fopen(fname, "r");
        int twordcount = 0, tpost = 0;
        vector<int> time_min;
        vector<double> lalist;
        vector<double> lolist;
        double locationla = 0, locationlo = 0;
        string addem;
        double avgla = 0, avglo = 0;
        avgt = 0;
        int tdate;
        while(fgets(l1, 140, eventfile)){
            fgets(l3, 260, eventfile);
            tpost++;
            int edate;
            char emonth[10], etime[20];
            double ela, elo;
            sscanf(l1, "%s%d%s%lf%lf", emonth, &edate, etime, &ela, &elo);
            tdate = edate;
            addem = emonth;
            //eventdate.push_back(edate);
            string addet = etime; eventtime.push_back(addet);
            locationla += ela;
            locationlo += elo;
            int dt, ht, mt;
            sscanf(etime, "%d:%d:%d", &dt, &ht, &mt);
            lalist.push_back(ela);
            avgla += ela;
            lolist.push_back(elo);
            avglo += elo;
            time_min.push_back(dt*60+ht);
            avgt += dt*60+ht;
        }
        avgla = avgla / tpost;
        avglo = avglo / tpost;
        avgt = avgt / tpost;
        eventtweets[fid] = tpost;
        double vardis = 0, vartime = 0;
        for(int i = 0; i < tpost; i++){
            double todis = distance(avgla, avglo, lalist[i], lolist[i]);
            vardis += todis * todis;
            vartime = vartime + (avgt - time_min[i]) * (avgt - time_min[i]);
        }
        sort(time_min.begin(), time_min.end());
        eventlength[fid] = time_min[(int)(0.9 * tpost)] - time_min[tpost / 10];
        tpost = tpost >= 1 ? tpost : 1;
        vardis = vardis / tpost;
        vartime = vartime / tpost;
        eventdate_.push_back(tdate);
        eventtime_.push_back(avgt);
        eventtimevar.push_back(vartime);
        eventla.push_back(avgla);
        eventmonth.push_back(addem);
        eventlo.push_back(avglo);
        eventdisvar.push_back(vardis);
        eventcount++;

        for(int i = 0; i < eventtime_.size() - 1; i++){
                //double temp1, temp2;
            double temp1 = avgt - eventtime_[i], temp2 = vartime - eventtimevar[i];
            //cout << fid << '\t' << i << endl;
            //cout << fabs(temp1) << '\t' << fabs(temp2) << '\t' << tdate << endl; // distance(avgla, avglo, eventla[i], eventlo[i]) << endl;   && distance(avgla, avglo, eventla[i], eventlo[i]) < 3
            //cout << eventtime_.size() << '\t' << eventtimevar.size() << endl;
            //cout << vartime << '\t' << tpost << '\t' << eventtimevar[i] << endl;
            //cout << tdate << endl;
            //cout << eventdate_.size() << '\t' << i << endl;
             //cout << eventdate_[i] << endl;
            if((tdate == eventdate_[i])){
                //cout << "combined" << endl;
                if(fabs(temp1) < 20){
//cout << "combined" << endl;
                    if((fabs(temp2) < ((vartime + eventtimevar[i]) / 10))){
                        cout << "combined" << endl;
                        eventcount--;
                        combined[fid] = true;
                        combinedto[fid] = i;
                        combinecount[i] ++;
                        break;

                    }
                }
            }
            //cout << '.' << endl;
        }
        fclose(eventfile);
        fprintf(varr, "%d\t%d\t%d\t%d::%lf\t%lf\t%lf\t%lf\t%lf\n", fid, eventcount, combinedto[fid], tdate, avgt, vartime / tpost, avgla, avglo, vardis / tpost);
    }
    //return 0;


    FILE *summary = fopen("event_summary.txt", "w");
    FILE *outhere = fopen("event_features.txt", "w");
    for(int fid = 0; fid < 2288; fid++){
        int tweetscount = 0;
        map<string, int>termcount;
        map<string, int>termoccur;
        vector<string> termlist;
        char fname[300];
        strcpy(fname, "Event\\");
        char forder[10];
        sprintf(forder, "%d", fid);
        strcat(fname, forder);
        FILE *eventfile = fopen(fname, "r");
        int twordcount = 0, tpost = 0;
        while(fgets(l1, 140, eventfile)){
            fgets(l3, 260, eventfile);
            tpost++;
        int k = 0;
        char wordcut[200];
        vector<string> scanstr;
        for(int i = 0; l3[i] != '\0'; i++){
            bool fw = true;
            if((l3[i] == ' ' || l3[i] == '"')){
                if(fw && (k > 0 || k < 15) && wordcut[0] != ' '){
                    wordcut[k] = '\0';
                    string nword = wordcut;
                    twordcount++;
                    if(termcount.find(nword) != termcount.end()){
                        termcount[nword]++;
                        bool repeat = true;
                        for(int temi = 0; temi < scanstr.size(); temi++){
                            if(nword == scanstr[temi]){
                                repeat = false;
                                break;
                            }
                        }
                        if(repeat)
                            termoccur[nword]++;
                        scanstr.push_back(nword);
                    }
                    else{
                        termcount[nword] = 1;
                        termoccur[nword] = 1;
                        termlist.push_back(nword);
                        scanstr.push_back(nword);
                    }
                }
                k = 0;
            }
            else if((l3[i] >= 'a' && l3[i] <= 'z') || (l3[i] >= '0' && l3[i] <= '9') || l3[i] == '@' || l3[i] == '#' || l3[i] == '\''){
                wordcut[k] = l3[i];
                k++;
            }
            else{
                fw = false;
                k++;
            }
        }
        }
        fclose(eventfile);
cout << "break\n" << endl;
        fprintf(summary, "%d\n", fid);
        eventfile = fopen(fname, "r");
        double locationla = 0, locationlo = 0;
        vector<int> eventdate;
        vector<string> eventtime;
        vector<string> eventmonth;
        vector<tweetlist> keytweet;
        int realcount = 0;
        while(fgets(l1, 140, eventfile)){
            fgets(l3, 260, eventfile);
            int edate;
            char emonth[10], etime[20];
            double ela, elo;
            sscanf(l1, "%s%d%s%lf%lf", emonth, &edate, etime, &ela, &elo);
            string addem = emonth; eventmonth.push_back(addem);
            eventdate.push_back(edate);
            string addet = etime; eventtime.push_back(addet);
            if(fabs(ela - 34.020790) > 0.00001 && fabs(elo + 118.411911) > 0.00001){      //34.020790	-118.411911
                locationla += ela;
                locationlo += elo;
                realcount++;
            }
            tweetlist newtweet;
            strcpy(newtweet.tweet, l3);
            newtweet.weight = 0;
        int k = 0;
        char wordcut[200];
        int tweetwords = 0;
        for(int i = 0; l3[i] != '\0'; i++){
            bool fw = true;
            if((l3[i] == ' ' || l3[i] == '"')){
                if(fw && (k > 0 || k < 15) && wordcut[0] != ' '){
                    wordcut[k] = '\0';
                    string nword = wordcut;
                    tweetwords ++;
                    newtweet.weight += ((double)termcount[nword]/twordcount)*(log2((double)termoccur[nword]/tpost));
                }
                k = 0;
            }
            else if((l3[i] >= 'a' && l3[i] <= 'z') || (l3[i] >= '0' && l3[i] <= '9') || l3[i] == '@' || l3[i] == '#' || l3[i] == '\''){
                wordcut[k] = l3[i];
                k++;
            }
            else{
                fw = false;
                k++;
            }
        }
        if(tweetwords < 3) tweetwords = 3;
        newtweet.weight = newtweet.weight / tweetwords;
        keytweet.push_back(newtweet);
        }
        fclose(eventfile);
        sort(keytweet.begin(), keytweet.end(), ktsorthelp);
        char wcom = 'f';
        if(combined[fid]) wcom = 't';
        if(realcount == 0) realcount = 1;
        fprintf(summary, "%s\t%d\t%s\t%lf\t%lf\n", eventmonth[eventmonth.size()/3].c_str(), eventdate[eventdate.size()/3], eventtime[eventtime.size()/3].c_str(), locationla / realcount, locationlo / realcount);
        fprintf(summary, "%c\t%d\t%d\t%d\n", wcom, eventtweets[fid], eventlength[fid], combinecount[fid]);
        if(fid == 2 || fid == 4 || fid == 5 || fid == 7 || fid ==  8 || fid == 12 || fid == 13 || fid == 37 || fid == 41 || fid == 2147 || fid == 48 || fid == 49 || fid == 118 || fid == 119 || fid == 129 || fid == 131 || fid == 134 || fid == 145 || fid == 146){
            fprintf(outhere, "%c\t%d\t%d\t%d\t%d\n", wcom, eventtweets[fid], eventlength[fid], combinecount[fid]+1, eventtweets[fid]*(combinecount[fid]+1));
        }

        for(int ki = 0; ki < tpost && ki < 5; ki++){
            fprintf(summary, "%s", keytweet[ki].tweet);
        }
        /*
        for(int i = 0; i < termlist.size(); i++){
            if(termcount[termlist[i]] > 10 && words[wordcount[termlist[i]]].cc < 0.4){
                fprintf(summary, "%s\t%d\n", termlist[i].c_str(), termcount[termlist[i]]);
            }
        }*/
        fprintf(summary, "\n\n\n");
    }

    return 0;
}

double distance(double lat1, double lon1, double lat2, double lon2) {
  double theta, dist;
  char unit = 'K';
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

/*
in = fopen("..\\UserAssign\\citytweets\\1", "r");
    while(fgets(l1, 140, in)){
        fgets(l2, 10, in);
        fgets(l3, 260, in);
        char id[30], month[20], time[20];
        int city, days, date;
        double la, lo;
        sscanf(l1, "%s%d%d%s%d%s%lf%lf", id, &city, &days, month, &date, time, &la, &lo);
        int k = 0;
        char wordcut[200];
        for(int i = 0; l3[i] != '\0'; i++){
            bool fw = true;
            if((l3[i] == ' ' || l3[i] == '"')){
                if(fw && (k > 0 || k < 15) && wordcut[0] != ' '){
                    wordcut[k] = '\0';
                    string nword = wordcut;
                    if(daylist[days].find(nword) != daylist[days].end()){
                        int fid = daylist[days][nword];
                        char fname[300];
                        strcpy(fname, "Event\\");
                        char forder[10];
                        sprintf(forder, "%d", fid);
                        strcat(fname, forder);
                        FILE *eventfile = fopen(fname, "at+");
                        fprintf(eventfile, "%s", l3);
                        fclose(eventfile);
                    }
                }
                k = 0;
            }
            else if((l3[i] >= 'a' && l3[i] <= 'z') || (l3[i] >= '0' && l3[i] <= '9') || l3[i] == '@' || l3[i] == '#' || l3[i] == '\''){
                wordcut[k] = l3[i];
                k++;
            }
            else{
                fw = false;
                k++;
            }
        }
    }
    */
