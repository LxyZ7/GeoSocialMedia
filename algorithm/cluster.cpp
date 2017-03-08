#include "cluster.h"
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include "clusterresult.h"
#include "networkgen.h"

#define REMOVE 10000000
#define STA 20
#define pi 3.1415926535


vector<clustering_node>Node_list;
clustering_node node;

vector<Adjacency_matrix>Adjacency_list;
Adjacency_matrix triple_tuple;

vector<adjacency_index>adindex;
adjacency_index insertindex;

vector<node_index>nodeindex;
node_index insertnodeindex;

extern vector<CommunityRecord>Community_list;
CommunityRecord community;

float sc_parameter;
extern int feature[NSIZE][NSIZE];
int TotalM;

void node_insert(int nid, float x, float y, float degree){
    node.x = x;
    node.y = y;
    node.nid = nid;
    node.degree = degree;
    node.group = (int)Node_list.size();
    Node_list.push_back(node);
    insertindex.begin = -1;
    insertindex.edge = 0;
    adindex.push_back(insertindex);
    insertnodeindex.begin = -1;
    nodeindex.push_back(insertnodeindex);
}

void adjacency_insert(int id1, int id2, float weight, int edge){
    triple_tuple.i = id1;
    triple_tuple.j = id2;
    triple_tuple.weight = weight;
    triple_tuple.edge = edge;
    triple_tuple.size = 0;
    triple_tuple.count = 0;
    Adjacency_list.push_back(triple_tuple);
    triple_tuple.i = id2;
    triple_tuple.j = id1;
    triple_tuple.weight = weight;
    triple_tuple.edge = edge;
    triple_tuple.size = 0;
    triple_tuple.count = 0;
    Adjacency_list.push_back(triple_tuple);
}

void Arrange_adjacency_list(){
    sort(Adjacency_list.begin(), Adjacency_list.end());
    int removecount = 0;
    int i;
    for(i = 0; i < Adjacency_list.size(); i++){
        if(Adjacency_list[i].i == REMOVE){
            removecount++;
        }
    }
    for(i = 0; i < removecount; i++){
        Adjacency_list.pop_back();
    }
    int temp = -1;
    for(i = 0; i < Adjacency_list.size(); i++){
        if(temp != Adjacency_list[i].i){
            temp = Adjacency_list[i].i;
            adindex[Adjacency_list[i].i].begin = i;
            if(i != 0){
                adindex[Adjacency_list[i-1].i].end = i - 1;
            }
        }
    }
    adindex[Adjacency_list[i-1].i].end = i - 1;
    //printf("adj: %d\n", i);
}

void Arrange_node_list(){
    sort(Node_list.begin(), Node_list.end());
    int temp = -1, i;
    for(i = 0; i < Node_list.size(); i++){
        if(temp != Node_list[i].group){
            temp = Node_list[i].group;
            nodeindex[Node_list[i].group].begin = i;
            if(i != 0){
                nodeindex[Node_list[i-1].group].end = i - 1;
            }
        }
    }
    nodeindex[Node_list[i-1].group].end = i - 1;
    //printf("no: %d\n", i);
}

double deg2rad(double deg) {
    return (deg * pi / 180);
}

double rad2deg(double rad) {
    return (rad * 180 / pi);
}

float gendistance(int x1, int y1, int x2, int y2){
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

int DistanceCalculator(int x1, int y1, int x2, int y2) {
    double lat1, lon1, lat2, lon2;
    if(x1 == x2 && y1 == y2) return 0;
    lat1 = (float)x1 / 1000.0;
    lat2 = (float)x2 / 1000.0;
    lon1 = (float)y1 / 1000.0;
    lon2 = (float)y2 / 1000.0;
    char unit = 'K';
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
    return ((int)(dist));
}

int Search_ij(int i, int j){
    int temp;
    int begin = adindex[i].begin, end = adindex[i].end;
    //printf("serchij %d %d\n", begin, end);
    while(begin <= end){
        temp = (begin+end) / 2;
        if(j == Adjacency_list[temp].j){
            return temp;
        }
        if(j < Adjacency_list[temp].j){
            end = temp - 1;
            continue;
        }
        if(j > Adjacency_list[temp].j){
            begin = temp + 1;
        }
    }
    return -1;
}


float Punish(int groupi, int groupj){
    float pu = 0;
    int iscan, jscan;
    for(iscan = nodeindex[groupi].begin; iscan <= nodeindex[groupi].end; iscan++){
        for(jscan = nodeindex[groupj].begin; jscan <= nodeindex[groupj].end; jscan++){
            //pu = pu + Node_list[iscan].degree * Node_list[jscan].degree / (2 * TotalM);
            //pu = pu + pow(2.73, -(DistanceCalculator(Node_list[iscan].x, Node_list[iscan].y, Node_list[jscan].x, Node_list[jscan].y)/AVEDISTANCE)) * sc_parameter * sqrt((Node_list[iscan].degree+0.01) * (Node_list[jscan].degree+0.01)) * Node_list[iscan].degree * Node_list[jscan].degree / (2 * TotalM);
            //pu = pu + pow(2.73, -(DistanceCalculator(Node_list[iscan].x, Node_list[iscan].y, Node_list[jscan].x, Node_list[jscan].y)/AVEDISTANCE)) * Node_list[iscan].degree * Node_list[jscan].degree / (2 * TotalM);
            
            //gen part
            //pu = pu + pow(2.73, -(gendistance(Node_list[iscan].x, Node_list[iscan].y, Node_list[jscan].x, Node_list[jscan].y)/AVEDISTANCE)) * Node_list[iscan].degree * Node_list[jscan].degree / (2 * TotalM);
            pu = pu + Node_list[iscan].degree * Node_list[jscan].degree / (2 * TotalM);
            //pu = pu + pow(2.73, -(gendistance(Node_list[iscan].x, Node_list[iscan].y, Node_list[jscan].x, Node_list[jscan].y)/AVEDISTANCE)) * sc_parameter * sqrt(Node_list[iscan].degree * Node_list[jscan].degree) * (Node_list[iscan].degree * Node_list[jscan].degree / (2 * TotalM));
            
        }//DistanceCalculator(Node_list[iscan].x, Node_list[iscan].y, Node_list[jscan].x, Node_list[jscan].y)
        //sqrt((Node_list[iscan].x - Node_list[jscan].x)*(Node_list[iscan].x - Node_list[jscan].x) + (Node_list[iscan].y-Node_list[jscan].y)*(Node_list[iscan].y-Node_list[jscan].y))
    }
    if(pu > 0)
    //if(pu > 0.5) printf("pu: %f\n", pu);
    return pu;
    return 0;
}

int numcluster;

void Community_detection(int numberofcluster){
    float maxQ;
    numcluster = numberofcluster;
    int i, merge = -1, mergei, mergej;
    int iscan, jscan, k, temp, randomnum;
    int stop = (int)Node_list.size(), control = 0;
    printf("Nodelist: %d\n", stop);
    for(control = 0; control < stop - numberofcluster; control++){
        Arrange_adjacency_list();
        Arrange_node_list();
        
        //Debug
        //for(i = 0; i < Adjacency_list.size(); i++) printf("A%d: %d %d %f\n", i, Adjacency_list[i].i, Adjacency_list[i].j, Adjacency_list[i].weight);
        //printf("\n");
        
        maxQ = -10000000;
        for(i = 0; i < Adjacency_list.size(); i++){
            if(Adjacency_list[i].weight > maxQ){
                merge = i;
                maxQ = Adjacency_list[i].weight;
            }
        }
        if(maxQ < 0){
            //printf("Stop, cluster: %d\n", stop - control);
            //break;
        }
        
        mergei = Adjacency_list[merge].i;
        mergej = Adjacency_list[merge].j;
        community.edgenumber = Adjacency_list[merge].edge + adindex[mergei].edge + adindex[mergej].edge;
        adindex[mergej].edge = community.edgenumber;
        
        iscan = adindex[mergei].begin;
        jscan = adindex[mergej].begin;
        while(iscan <= adindex[mergei].end || jscan <= adindex[mergej].end){ //Update Q
            //printf("scan i, j: %d %d (%d %d)\n", iscan, jscan, adindex[mergei].end, adindex[mergej].end);
            if(Adjacency_list[iscan].j == mergej && iscan <= adindex[mergei].end){
                //Adjacency_list[iscan].i = REMOVE;
                iscan++;
                continue;
            }
            if(Adjacency_list[jscan].j == mergei && jscan <= adindex[mergej].end){
                //Adjacency_list[jscan].i = REMOVE;
                jscan++;
                continue;
            }
            if((Adjacency_list[iscan].j < Adjacency_list[jscan].j || jscan > adindex[mergej].end) && iscan <= adindex[mergei].end){
                k = Adjacency_list[iscan].j;
                Adjacency_list[iscan].i = mergej;
                Adjacency_list[iscan].weight = Adjacency_list[iscan].weight - Punish(mergej, k);
                //Adjacency_list[iscan].edge = Adjacency_list[iscan].edge + Adjacency_list[merge].edge;
                temp = Search_ij(k, mergei);
                Adjacency_list[temp].j = mergej;
                Adjacency_list[temp].weight = Adjacency_list[iscan].weight;
                //Adjacency_list[temp].edge = Adjacency_list[iscan].edge;
                //printf("w: %f %f\n", Adjacency_list[iscan].weight, Punish(mergej, k));
                iscan++;
                continue;
            }
            if((Adjacency_list[iscan].j > Adjacency_list[jscan].j || iscan > adindex[mergei].end) && jscan <= adindex[mergej].end){
                k = Adjacency_list[jscan].j;
                Adjacency_list[jscan].weight = Adjacency_list[jscan].weight - Punish(mergei, k);
                //Adjacency_list[jscan].edge = Adjacency_list[jscan].edge + Adjacency_list[merge].edge;
                temp = Search_ij(k, mergej);
                Adjacency_list[temp].weight = Adjacency_list[jscan].weight;
                //Adjacency_list[temp].edge = Adjacency_list[jscan].edge;
                //printf("w: %f %f\n", Adjacency_list[jscan].weight, Punish(mergei, k));
                jscan++;
                continue;
            }
            if(Adjacency_list[iscan].j == Adjacency_list[jscan].j && iscan <= adindex[mergei].end && jscan <= adindex[mergej].end){
                Adjacency_list[jscan].weight = Adjacency_list[jscan].weight + Adjacency_list[iscan].weight;
                Adjacency_list[jscan].edge = Adjacency_list[jscan].edge + Adjacency_list[iscan].edge;
                temp = Search_ij(Adjacency_list[jscan].j, mergej);
                Adjacency_list[temp].weight = Adjacency_list[jscan].weight;
                Adjacency_list[temp].edge = Adjacency_list[jscan].edge;
                //printf("w: %f\n", Adjacency_list[jscan].weight);
                iscan++;
                jscan++;
            }
        }
        //Update node list and remove unused elements in Q
        for(temp = nodeindex[mergei].begin; temp <= nodeindex[mergei].end; temp++){
            Node_list[temp].group = mergej;
        }
        if(control % 100 == 0)
            printf("%d %f\n", control, maxQ);
        //int testi;
        //for(testi = 0; testi < Node_list.size(); testi++) printf("%d:%d  ", Node_list[testi].nid, Node_list[testi].group);
        //printf("\n");
        //printf("%d (%d %d)\n", mergei, nodeindex[mergei].begin, nodeindex[mergei].end);
        for(iscan = adindex[mergei].begin; iscan <= adindex[mergei].end; iscan++){
            if(Adjacency_list[iscan].i == mergei){
                Adjacency_list[iscan].i = REMOVE;
                temp = Search_ij(Adjacency_list[iscan].j, mergei);
                Adjacency_list[temp].i = REMOVE;
                //printf("test: %d %d\n", iscan, temp);
            }
        }
        if((nodeindex[mergei].end + 2 + nodeindex[mergej].end - nodeindex[mergei].begin - nodeindex[mergej].begin) <= 100){
            community.size = 0;
            if(community.size < 0) printf("Community Size ERROR! %d \n", community.size);
            i = 0;
            for(temp = nodeindex[mergei].begin; temp <= nodeindex[mergei].end; temp++){
                community.nodeid[i] = Node_list[temp].nid;
                //srand(temp * temp);
                //randomnum = (int)random();
                //randomnum = randomnum % Node_list.size();
                community.latitude[i] = Node_list[temp].x;//Node_list[temp].x;
                community.longitude[i] = Node_list[temp].y;
                community.size++;
                i++;
            }
            for(temp = nodeindex[mergej].begin; temp <= nodeindex[mergej].end; temp++){
                community.nodeid[i] = Node_list[temp].nid;
                //srand(temp * temp);
                //randomnum = (int)random();
                //randomnum = randomnum % Node_list.size();
                community.latitude[i] = Node_list[temp].x;
                community.longitude[i] = Node_list[temp].y;
                community.size++;
                i++;
            }
            Community_list.push_back(community);
            //printf("E: %d %d %d %d\n", mergei, mergej, community.size, community.edgenumber);
        }
    }
}



void Accuracy(){
    Arrange_node_list();
    int fea[100];
    int group, temp, i = 0, j, totalcount = 0;
    group = Node_list[0].group;
    while (i < Node_list.size()){
        for(j = 0; j < 100; j++) fea[j] = 0;
        for(; i < Node_list.size() && Node_list[i].group == group; i++){
            temp = Node_list[i].nid;
            fea[feature[temp / NSIZE][temp % NSIZE]]++;
        }
        temp = 0;
        for(j = 0; j < 100; j++){
            if(fea[j] > temp){
                temp = fea[j];
            }
        }
        totalcount = totalcount + temp;
        if(i < Node_list.size()){
            group = Node_list[i].group;
        }
    }
    printf("Total: %d Accuracy: %f\n", i, (float)totalcount / Node_list.size());
}




















