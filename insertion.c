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

// if handling insertion of leaf node, calculate hv when calling function
// else feed in LHV of non leaf node
NODE handleOverflow(NODE L, Rect r, int h){

    // Entry to be inserted
    ENTRY new_entry = (ENTRY) malloc(sizeof(Entry));
    new_entry->child = NULL;
    new_entry->LHV = h;
    new_entry->MBR = r;

    ENTRY* s = L->parent->all_entries;
    int no_of_entries = 1; // node to be inserted;
    int no_of_nodes = 0;

    for(int i = 0; i < 4; i++) {
        no_of_entries += count_entries(s[i]->child);
    }

    for(int i = 0; i < 4; i++) {
        if(s[i] == NULL) no_of_nodes = i;
    }

    ENTRY* e_arr = (ENTRY) malloc(sizeof(ENTRY) * no_of_entries);

    int i = 0, j = 0, k = 0; // i - counter for e_arr pointer,
    bool new_entry_inserted = false;

    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            if(s[i]->child->all_entries[j] != NULL) {
                if(new_entry_inserted == false) {
                    if(new_entry->LHV <= s[i]->child->all_entries[j]->LHV) {
                        e_arr[k] = new_entry;
                        new_entry_inserted = true;
                        k++;
                        continue;
                    }
                }
                e_arr[k] = s[i]->child->all_entries[j];
                k++;
            }
        }
    }
    
    if(no_of_entries <= (no_of_nodes) * 4) {
        // evenly distribute existing entries into existing nodes
        // note :- av can never be less than 2 since m is 2
        int av = (no_of_entries) / (no_of_nodes);
        int rem = (no_of_entries) % (no_of_nodes);

        

        return NULL; 
    } else {
        // create new node
        NODE new_node = (NODE) malloc(sizeof(struct Node));
        // evenly distribute all entries into all 5 nodes

        int i;
        int j = 0;
        int k = 0; // current entry in parent node
        for(i = 0; i < no_of_nodes * 4; i++) {
            s[k]->child->all_entries[j] = e_arr[i];
            j++;
            if(j == 4) {
                j = 0;
                k++;
            }
        }

        j = 0;
        while(i < no_of_entries) {
            new_node->all_entries[j] = e_arr[i];
            i++; j++;
        }

        return new_node;
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