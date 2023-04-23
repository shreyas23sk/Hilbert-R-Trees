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