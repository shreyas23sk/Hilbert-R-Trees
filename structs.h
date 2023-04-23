#include <stdio.h>
#include <stdlib.h>

typedef struct Point Point;
struct Point{
    int x;
    int y;
};

typedef struct Rect Rect;
struct Rect {
    Point top_left;
    Point top_right;
};

// page 4, sec 3.1
typedef struct Node* NODE;
struct Node {
    Rect MBR; // minimum bounding rectangle for all the child nodes of this entry
    NODE child;
    int LHV; // largest hilbert value of data rectangles of the subtree (NOT MBR)
};