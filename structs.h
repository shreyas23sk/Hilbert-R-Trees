#ifndef STRUCTS_H
#define STRUCTS_H
#include "bool.h"
#include <stdlib.h>
#include <stdio.h>
typedef unsigned long long int;

typedef struct Point Point;
struct Point{
    int x;
    int y;
};

typedef struct Rect Rect;
struct Rect { // top_left == bottom_right in case of a single 2D point (degenerate rectangle)
    Point top_right;
    Point bottom_left;
};

typedef struct Node* NODE;
typedef struct Entry Entry;
struct Entry {
    Rect MBR; // minimum bounding rectangle for all the child nodes of this entry
    NODE child;
    int LHV; // largest hilbert value of data rectangles of the subtree (NOT MBR)
};
typedef struct Entry* ENTRY;
struct Node {
    NODE parent; // the node which contains the entry which is parent to the current node
    ENTRY all_entries[4];
};


typedef struct HRTree* HRT;
struct HRTree{
    NODE root;
};

// calculate Hilbert Value of the MID-POINT of data rectangles
// but data rectangles here are only single 2D points
int calculate_hilbert_value(Rect);

bool isLeaf(NODE);

// obtain the LHV of a particular entry 
//by taking the maximum LHV from the entries of the child node
void set_lhv(ENTRY);

// since all leaf nodes in the structure to be implemented are degenerate rectangles,
// we can simply return the Point if found, Nint if not.
Rect* search(HRT, Rect);
NODE createNewNodeOfTree();

// insertion functions

// finds a leaf node with a LHV which is over the the HV of the rect and is minimum
NODE chooseLeaf(HRT, Rect, int);
NODE HandleOverflow(HRT,NODE, NODE, Rect, int);
void AdjustTree(HRT, NODE, NODE);
void insert(HRT, Rect);
#endif