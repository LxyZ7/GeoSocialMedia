#ifndef CLUSTER_H
#define CLUSTER_H
#include <vector>

#define AVEDISTANCE 10
using namespace std;

int Search_ij(int i, int j);
void node_insert(int nid, float x, float y, float degree);
void adjacency_insert(int id1, int id2, float weight, int edge);
void Community_detection(int numberofcluster);
float Punish(int groupi, int groupj);
double rad2deg(double rad);
double deg2rad(double deg);
void Arrange_node_list();
void Arrange_adjacency_list();
int DistanceCalculator(int x1, int y1, int x2, int y2);
void Accuracy();
float gendistance(int x1, int y1, int x2, int y2);

typedef struct clustering_node{
    float x, y;
    float degree;
    int group;
    int nid;
    bool operator < (const clustering_node &other)const{
        if(group != other.group) return group < other.group;
        return nid < other.nid;
    }
}clustering_node;
//The node id (nid) must be from 0 to n!

typedef struct Adjacency_matrix{
    int i, j;
    float weight;
    int edge;
    int size;
    int count;
    bool operator < (const Adjacency_matrix &other)const{
        if(i != other.i) return i < other.i;
        return j < other.j;
    }
}Adjacency_matrix;
// i must less than j!

typedef struct node_index{
    int begin, end;
    int size;
}node_index;

typedef struct adjacency_index{
    int begin, end;
    int size;
    int edge;
}adjacency_index;



#endif