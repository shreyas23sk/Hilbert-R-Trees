#include "structs.h"

int count_entries(NODE n) {
    for(int i = 0; i < 4; i++) {
        if(n->all_entries[i] == NULL) return i; 
    }
    return 4;
}

NODE chooseLeaf(HRT ht, Rect r, int h)
{
    NODE n = ht->root;

    while (isLeaf(n) == false)
    {
        NODE temp;
        int min_lhv = 10000; // arbitrary big number, clean up later
        for (int i = 0; i < 4; i++)
        {
            if (n->all_entries[i]->LHV > h)
            {
                if (n->all_entries[i]->LHV < min_lhv)
                {
                    temp = n->all_entries[i]->child;
                    min_lhv = n->all_entries[i]->LHV;
                }
            }
        }

        n = temp;
    }

    return n;
}

NODE handleOverflow(NODE leaf, Rect r){
    int h = calculate_hilbert_value(r);

    // Entry to be inserted
    ENTRY new_entry = (ENTRY) malloc(sizeof(Entry));
    new_entry->child = NULL;
    new_entry->LHV = h;
    new_entry->MBR = r;

    NODE siblings[2];

    NODE p = leaf->parent;
    int l_index;
    for(int i = 0; i < 4; i++) {
        if(p->all_entries[i]->child == leaf) l_index = i;
    }

    if(l_index == 0) { 
        siblings[0] = leaf;
        siblings[1] = p->all_entries[1]->child;
    }
    else {
        siblings[0] = p->all_entries[l_index - 1]->child;
        siblings[1] = leaf;
    }

    if(count_entries(siblings[0]) + count_entries(siblings[1]) != 8) { // not all nodes are full
        ENTRY all_entries_in_set[8];

        for(int i = 0; i < 2; i++) {
            for(int j = 0; j < 4; j++) {
                all_entries_in_set[i * 4 + j] = siblings[i]->all_entries[j];
            }
        }

        // insert new_entry
        ENTRY prev, curr;
        bool new_entry_inserted = false;
        for(int i = 0; i < 8; i++) {
            if(new_entry_inserted == false) {
                if(all_entries_in_set[i]->LHV > new_entry->LHV) {
                    prev = all_entries_in_set[i];
                    all_entries_in_set[i] = new_entry;
                    new_entry_inserted = true;
                }
            } else {
                curr = all_entries_in_set[i];
                all_entries_in_set[i] = prev;
                prev = curr;
            }
        }

        for(int i = 0; i < 2; i++) {
            for(int j = 0; j < 4; j++) {
                siblings[i]->all_entries[j] = all_entries_in_set[i * 4 + j];
            }
        }
    } else {

    }
}

void insert(HRT ht, Rect r)
{
    int h = calculate_hilbert_value(r);
    NODE leaf = chooseLeaf(ht, r, h);
    if(leaf->all_entries[3]==NULL){
        for(int i=0;i<4;i++){
            if(leaf->all_entries[i]!=NULL && leaf->all_entries[i]->LHV < h){
                continue;
            }
            else{
                if(leaf->all_entries[i]==NULL){
                    leaf->all_entries[i]->MBR=r;
                    leaf->all_entries[i]->LHV=h;
                }
                else{
                    ENTRY temp=leaf->all_entries[i];
                    leaf->all_entries[i]->MBR=r;
                    leaf->all_entries[i]->LHV=h;
                    for(int j=i+1;j<4;j++){
                        ENTRY temp2=leaf->all_entries[j];
                        leaf->all_entries[j]=temp;
                        temp=temp2;
                    }
                }
                break;
            }
        }
    }
    else{
        NODE leaf2=HandleOverflow(leaf,r);
    }
}