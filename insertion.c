#include "structs.h"
#include "bool.h"

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

NODE HandleOverflow(NODE leaf, Rect r){
    
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