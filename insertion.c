#include "structs.h"

NODE chooseLeaf(HRT ht, Rect r, int h)
{
    NODE n = ht->root;

    while(isLeaf(n) == false) {
        NODE temp;
        int min_lhv = 10000; // arbitrary big number, clean up later
        for(int i = 0; i < 4; i++) {
            if(n->all_entries[i]->LHV > h) {    
                if(n->all_entries[i]->LHV < min_lhv) {
                    temp = n->all_entries[i]->child;
                    min_lhv = n->all_entries[i]->LHV;
                }
            }
        }

        n = temp;
    }

    return n;
}