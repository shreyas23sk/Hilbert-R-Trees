#include "structs.h"
#include <limits.h>
#define MIN(a, b) a < b ? a : b
#define MAX(a, b) a > b ? a : b

int count_entries(NODE n)
{
    for (int i = 0; i < 4; i++)
    {
        if (n->all_entries[i] == NULL)
            return i;
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

// if handling insertion of leaf node, calculate hv when calling function, call with n = NULL
// else feed in LHV of non leaf node, and treat n as the child node of new entry to be inserted
NODE HandleOverflow(NODE L, NODE n, Rect r, int h)
{

    // Entry to be inserted
    ENTRY new_entry = (ENTRY)malloc(sizeof(Entry));
    new_entry->child = n;
    new_entry->LHV = h;
    new_entry->MBR = r;

    ENTRY *s = L->parent->all_entries;
    int no_of_entries = 1; // node to be inserted;
    int no_of_nodes = 0;

    for (int i = 0; i < 4; i++)
    {
        no_of_entries += count_entries(s[i]->child);
    }

    for (int i = 0; i < 4; i++)
    {
        if (s[i] == NULL)
            no_of_nodes = i;
    }

    ENTRY *e_arr = (ENTRY)malloc(sizeof(ENTRY) * no_of_entries);

    int i = 0, j = 0, k = 0; // i - counter for e_arr pointer,
    bool new_entry_inserted = false;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (s[i]->child->all_entries[j] != NULL)
            {
                if (new_entry_inserted == false)
                {
                    if (new_entry->LHV <= s[i]->child->all_entries[j]->LHV)
                    {
                        e_arr[k] = new_entry;
                        new_entry_inserted = true;
                        k++;
                        j--;
                        continue;
                    }
                }
                e_arr[k] = s[i]->child->all_entries[j];
                k++;
            }
        }
    }

    if (no_of_entries <= (no_of_nodes)*4)
    {
        // evenly distribute existing entries into existing nodes
        // note :- av can never be less than 2 since m is 2
        int av = (no_of_entries) / (no_of_nodes);
        int rem = (no_of_entries) % (no_of_nodes);

        int j = 0;
        int k = 0; // current entry in parent node
        for (int i = 0; i < no_of_entries; i++)
        {
            s[k]->child->all_entries[j] = e_arr[i];
            if (e_arr[i]->child != NULL)
                e_arr[i]->child->parent = L;
            j++;
            if (j >= av)
            {
                if (rem > 0 && j < 4)
                    rem--;
                else
                {
                    k++;
                    j = 0;
                }
            }
        }

        return NULL;
    }
    else
    {
        // create new node
        NODE new_node = (NODE)malloc(sizeof(struct Node));
        // evenly distribute all entries into all 5 nodes

        int av = (no_of_entries) / (no_of_nodes);
        int rem = (no_of_entries) % (no_of_nodes);
        int i;
        int j = 0;
        int k = 0; // current entry in parent node
        for (i = 0; i < no_of_nodes * av; i++)
        {
            s[k]->child->all_entries[j] = e_arr[i];
            if (e_arr[i]->child != NULL)
                e_arr[i]->child->parent = L;
            j++;
            if (j >= av)
            {
                if (rem > 0 && j < 4)
                    rem--;
                else
                {
                    k++;
                    j = 0;
                }
            }
        }

        j = 0;
        while (i < no_of_entries)
        {
            new_node->all_entries[j] = e_arr[i];
            i++;
            j++;
        }

        return new_node;
    }
}

bool checkSplit(NODE leaf, Rect r, int h)
{
    if (leaf->all_entries[3] == NULL)
    {
        for (int i = 0; i < 4; i++)
        {
            if (leaf->all_entries[i] != NULL && leaf->all_entries[i]->LHV < h)
            {
                continue;
            }
            else
            {
                if (leaf->all_entries[i] == NULL)
                {
                    leaf->all_entries[i]->MBR = r;
                    leaf->all_entries[i]->LHV = h;
                }
                else
                {
                    ENTRY temp = leaf->all_entries[i];
                    leaf->all_entries[i]->MBR = r;
                    leaf->all_entries[i]->LHV = h;
                    for (int j = i + 1; j < 4; j++)
                    {
                        ENTRY temp2 = leaf->all_entries[j];
                        leaf->all_entries[j] = temp;
                        temp = temp2;
                    }
                }
                break;
            }
        }

        return true;
    } else {
        return false;
    }
}

void AdjustTree(NODE S, NODE NN)
{
    if (S == NULL)
        return;
    else
    {
        NODE P = S->parent;
        if(NN != NULL) {
            
        }

        for (int i = 0; i < 4; i++)
        {
            ENTRY e = P->all_entries[i];
            int minx = INT_MAX, maxx = INT_MIN, miny = INT_MAX, maxy = INT_MIN, lhv = 0;
            if (e != NULL)
            {
                for (int j = 0; j < 4; j++)
                {
                    ENTRY ec = e->child->all_entries[j];
                    if (ec != NULL)
                    {
                        minx = MIN(minx, ec->MBR.bottom_left.x);
                        miny = MIN(miny, ec->MBR.bottom_left.y);
                        maxx = MAX(maxx, ec->MBR.top_right.x);
                        maxy = MAX(maxy, ec->MBR.top_right.y);
                        lhv = MAX(lhv, ec->LHV);
                    }
                }
            }
            Rect *new_MBR = (Rect *)malloc(sizeof(Rect));
            new_MBR->bottom_left.x = minx;
            new_MBR->bottom_left.y = miny;
            new_MBR->top_right.x = maxx;
            new_MBR->top_right.y = miny;
            e->MBR = *new_MBR;
            e->LHV = lhv;
        }
        AdjustTree(P->parent);
    }
}

void insert(HRT ht, Rect r)
{
    int h = calculate_hilbert_value(r);
    NODE leaf = chooseLeaf(ht, r, h);
    if (leaf->all_entries[3] == NULL)
    {
        for (int i = 0; i < 4; i++)
        {
            if (leaf->all_entries[i] != NULL && leaf->all_entries[i]->LHV < h)
            {
                continue;
            }
            else
            {
                if (leaf->all_entries[i] == NULL)
                {
                    leaf->all_entries[i]->MBR = r;
                    leaf->all_entries[i]->LHV = h;
                }
                else
                {
                    ENTRY temp = leaf->all_entries[i];
                    leaf->all_entries[i]->MBR = r;
                    leaf->all_entries[i]->LHV = h;
                    for (int j = i + 1; j < 4; j++)
                    {
                        ENTRY temp2 = leaf->all_entries[j];
                        leaf->all_entries[j] = temp;
                        temp = temp2;
                    }
                }
                break;
            }
        }
        AdjustTree(leaf->parent, NULL);
    }
    else
    {
        NODE leaf2 = HandleOverflow(leaf, NULL, r, h);
        NODE Np = leaf->parent;
        if (leaf2 == NULL)
            AdjustTree(Np, NULL);
        else
        {
        }
    }
}