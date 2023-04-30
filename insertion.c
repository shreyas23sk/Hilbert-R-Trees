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

ENTRY findMBR(NODE P)
{
    printf("inside findMBR\n");
    ENTRY e = (ENTRY)malloc(sizeof(Entry));
    int minx = INT_MAX, maxx = INT_MIN, miny = INT_MAX, maxy = INT_MIN, lhv = 0;
    for (int j = 0; j < 4; j++)
    {
        ENTRY ec = P->all_entries[j];
        if (ec != NULL)
        {
            minx = MIN(minx, ec->MBR.bottom_left.x);
            miny = MIN(miny, ec->MBR.bottom_left.y);
            maxx = MAX(maxx, ec->MBR.top_right.x);
            maxy = MAX(maxy, ec->MBR.top_right.y);
            lhv = MAX(lhv, ec->LHV);
        }
    }
    Rect *new_MBR = (Rect *)malloc(sizeof(Rect));
    new_MBR->bottom_left.x = minx;
    new_MBR->bottom_left.y = miny;
    new_MBR->top_right.x = maxx;
    new_MBR->top_right.y = miny;
    e->MBR = *new_MBR;
    e->LHV = lhv;
    e->child = P;
    return e;
}

NODE chooseLeaf(HRT ht, Rect r, int h)
{
    NODE n = ht->root;

    while (isLeaf(n) == false)
    {
        printf("inside chooseLeaf\n");
        NODE temp = NULL;
        NODE last_non_null_node = NULL;
        int min_lhv = INT_MAX;
        for (int i = 0; i < 4; i++)
        {
            if (n->all_entries[i] != NULL)
            {
                last_non_null_node = n->all_entries[i]->child;
                if (n->all_entries[i]->LHV > h)
                {
                    if (n->all_entries[i]->LHV < min_lhv)
                    {
                        temp = n->all_entries[i]->child;
                        min_lhv = n->all_entries[i]->LHV;
                    }
                }
            }
        }

        // assigning temp the node with the highest value of LHV in case of no node having higher value than h
        if (temp == NULL)
            temp = last_non_null_node;
        n = temp;
    }

    return n;
}

// if handling insertion of leaf node, calculate hv when calling function, call with n = NULL
// else feed in LHV of non leaf node, and treat n as the child node of new entry to be inserted
NODE HandleOverflow(HRT ht, NODE L, NODE n, Rect r, int h)
{

    // Entry to be inserted
    ENTRY new_entry = (ENTRY)malloc(sizeof(Entry));
    new_entry->child = n;
    new_entry->LHV = h;
    new_entry->MBR = r;
    // Here root is splitting So we are handling this case seperately to create a new root
    if (L->parent == NULL)
    {
        printf("inside root splitting\n");
        ENTRY *s = (ENTRY *)malloc(sizeof(Entry) * 5);
        int j = 0;
        int k = 0;
        for (int i = 0; i < 4; i++)
        {
            if (L->all_entries[j]->LHV < new_entry->LHV)
            {
                s[k] = L->all_entries[j];
                k++;
                j++;
            }
            else
            {
                s[k] = new_entry;
                k++;
                break;
            }
        }
        for (int i = j; i < 4; i++)
        {
            s[k] = L->all_entries[j];
            k++;
            j++;
        }
        for (int i = 0; i < 3; i++)
        {
            L->all_entries[i] = s[i];
        }
        NODE new_node = createNewNodeOfTree();
        new_node->all_entries[0] = s[3];
        new_node->all_entries[1] = s[4];
        NODE new_root = createNewNodeOfTree();
        new_root->all_entries[0] = findMBR(L);
        new_root->all_entries[1] = findMBR(new_node);
        new_root->parent = NULL;
        new_node->parent = new_root;
        L->parent = new_root;
        ht->root = new_root;

        return NULL;
    }
    else
    {

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
        if (no_of_nodes == 0)
            no_of_nodes = 4;

        ENTRY *e_arr = (ENTRY *)malloc(sizeof(ENTRY) * no_of_entries);

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
            new_node->parent = NULL;
            // evenly distribute all entries into all 5 nodes

            int av = (no_of_entries) / (no_of_nodes + 1);
            int rem = (no_of_entries) % (no_of_nodes + 1);
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
}

bool checkSplit(HRT ht, NODE leaf, Rect r, int h)
{
    printf("inside checksplit\n");
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
                    ENTRY e = (ENTRY)malloc(sizeof(Entry));
                    e->MBR = r;
                    e->child = NULL;
                    e->LHV = h;
                    leaf->all_entries[i] = e;
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
        printf("end of checksplit\n");
        AdjustTree(ht, leaf->parent, NULL);
    }
    // else {
    //     return false;
    // }
    else
    {
        printf("inside handleoverflow\n");
        NODE leaf2 = HandleOverflow(ht, leaf, NULL, r, h);
        printf("outside handleoverflow\n");
        NODE Np = leaf->parent;
        if (leaf2 == NULL)
            AdjustTree(ht, Np, NULL);
        else
        {
            AdjustTree(ht, Np, leaf2);
        }
    }
}

void AdjustTree(HRT ht, NODE S, NODE NN)
{
    if (S == NULL)
        return;
    else
    {
        printf("inside adjustree\n");
        // NODE P = S->parent;
        if (NN != NULL)
        {
            for (int i = 0; i < 4; i++)
            {
                if (NN->all_entries[i] != NULL)
                    checkSplit(ht, S, NN->all_entries[i]->MBR, NN->all_entries[i]->LHV);
            }
        }

        for (int i = 0; i < 4; i++)
        {
            ENTRY e = S->all_entries[i];
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
                Rect *new_MBR = (Rect *)malloc(sizeof(Rect));
                new_MBR->bottom_left.x = minx;
                new_MBR->bottom_left.y = miny;
                new_MBR->top_right.x = maxx;
                new_MBR->top_right.y = miny;
                e->MBR = *new_MBR;
                e->LHV = lhv;
            }
        }
        AdjustTree(ht, S->parent, NULL);
    }
}

void insert(HRT ht, Rect r)
{
    int h = calculate_hilbert_value(r);
    printf("inside insert %d\n", h);
    if (ht->root->all_entries[0] == NULL)
    {
        ENTRY e = (ENTRY)malloc(sizeof(Entry));
        e->MBR = r;
        e->LHV = h;
        e->child = NULL;
        ht->root->all_entries[0] = e;
        return;
    }
    NODE leaf = chooseLeaf(ht, r, h);
    checkSplit(ht, leaf, r, h);
    // if (leaf->all_entries[3] == NULL)
    // {
    //     for (int i = 0; i < 4; i++)
    //     {
    //         if (leaf->all_entries[i] != NULL && leaf->all_entries[i]->LHV < h)
    //         {
    //             continue;
    //         }
    //         else
    //         {
    //             if (leaf->all_entries[i] == NULL)
    //             {
    //                 leaf->all_entries[i]->MBR = r;
    //                 leaf->all_entries[i]->LHV = h;
    //             }
    //             else
    //             {
    //                 ENTRY temp = leaf->all_entries[i];
    //                 leaf->all_entries[i]->MBR = r;
    //                 leaf->all_entries[i]->LHV = h;
    //                 for (int j = i + 1; j < 4; j++)
    //                 {
    //                     ENTRY temp2 = leaf->all_entries[j];
    //                     leaf->all_entries[j] = temp;
    //                     temp = temp2;
    //                 }
    //             }
    //             break;
    //         }
    //     }
    //     AdjustTree(leaf->parent, NULL);
    // }
    // else
    // {
    //     NODE leaf2 = HandleOverflow(leaf, NULL, r, h);
    //     NODE Np = leaf->parent;
    //     if (leaf2 == NULL)
    //         AdjustTree(Np, NULL);
    //     else
    //     {
    //         AdjustTree(Np, leaf2);
    //     }
    // }
}