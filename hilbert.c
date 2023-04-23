#include "structs.h"

// COPIED FROM WIKIPEDIA - change to avoid plag
void rot(int n, int *x, int *y, int rx, int ry) {
    if (ry == 0) {
        if (rx == 1) {
            *x = n-1 - *x;
            *y = n-1 - *y;
        }

        //Swap x and y
        int t  = *x;
        *x = *y;
        *y = t;
    }
}

int xy2d (int n, int x, int y) {
    int rx, ry, s, d=0;
    for (s=n/2; s>0; s/=2) {
        rx = (x & s) > 0;
        ry = (y & s) > 0;
        d += s * s * ((3 * rx) ^ ry);
        rot(n, &x, &y, rx, ry);
    }
    return d;
}


int calculate_hilbert_value(Point p) {
    return xy2d(20, p.x, p.y);
}

int get_lhv(Entry e) { // UNTESTED
    int lhv = 0;
    NODE c = e->child;
    for(int i = 0; i < 4; i++) {
        if(c.all_entries[i] != NULL) {
            int hv = calculate_hilbert_value(c.all_entries[i]);
            if(hv > lhv) lhv = hv;
        }
    }

    return lhv;
}