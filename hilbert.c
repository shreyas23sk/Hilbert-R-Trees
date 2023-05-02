#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#define MIN(a, b) a < b ? a : b
#define MAX(a, b) a > b ? a : b

// global variables
int check = 0;
int hilbert_N = 0;

// bool declaration
typedef enum
{
    false,
    true
} bool;

typedef unsigned long long ull;

typedef struct Point Point; // Point in rectangle
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
    NODE child; // points to the node whose MBRs are enclosed by this entry
    ull LHV; // largest hilbert value of data rectangles of the subtree (NOT MBR)
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
ull calculate_hilbert_value(Rect);

// helper functions
bool isLeaf(NODE);
void printNode(NODE);
void printChildNodes(NODE);
int count_entries(NODE);
ENTRY findMBR(NODE);
bool intersects(Rect, Rect);
void printRect(Rect);

// obtain the LHV of a particular entry 
//by taking the maximum LHV from the entries of the child node
void set_lhv(ENTRY);


// initializing functions
NODE createNewNodeOfTree();
Rect* createNewRect(Point);
Point* createNewPoint(int, int);

// search functions
// search takes in a node, a rect and an accumulator variable initialized in search_wrapper
void search(NODE, Rect, int*);
void search_wrapper(HRT, Rect);

// pre order traversal
// takes an accumulator variable to print the level of the current ENTRY in the tree
void pre_order_traversal(NODE, int);

// insertion functions
// finds a leaf node with a LHV which is over the the HV of the rect and is minimum
NODE chooseLeaf(HRT, Rect, ull);
NODE HandleOverflow(HRT,NODE, NODE, Rect, ull);
void AdjustTree(HRT, NODE, NODE);
void insert(HRT, Rect);


void printNode(NODE n) {
    for(int i = 0; i < 4; i++) {
        if(n->all_entries[i] != NULL) {
            printf("%lld ", n->all_entries[i]->LHV);
        } else {
            printf("NULL, ");
        }
    }
    printf("\n");
}

void printChildNodes(NODE n) {
    printf("Printing Child nodes :\n");
    for(int i = 0; i < 4; i++) {
        if(n->all_entries[i] != NULL && n->all_entries[i]->child != NULL) {
            printNode(n->all_entries[i]->child);
        } else if(n->all_entries[i] != NULL){
            printf("NO CHILD\n");
        } else {
            printf("NO ENTRY\n");
        }
    }
}

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
    //printf("inside findMBR\n");
    ENTRY e = (ENTRY)malloc(sizeof(Entry));
    int minx = INT_MAX, maxx = INT_MIN, miny = INT_MAX, maxy = INT_MIN;
    ull lhv = 0;
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
    new_MBR->top_right.y = maxy;
    e->MBR = *new_MBR;
    e->LHV = lhv;
    e->child = P;
    return e;
}

NODE chooseLeaf(HRT ht, Rect r, ull h)
{
    // Step C1: Initialize N to root node
    NODE n = ht->root;

    while (isLeaf(n) == false) // C2: If n is leaf node return n
    {
        NODE temp = NULL;
        NODE last_non_NULL_node = NULL;
        ull min_lhv = (unsigned long long) 1e15;
        for (int i = 0; i < 4; i++)  // C3 : Choose child node of entry with minimum LHV greater than h 
        {
            if (n->all_entries[i] != NULL)
            {
                last_non_NULL_node = n->all_entries[i]->child;
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
            temp = last_non_NULL_node;
        n = temp;
    }
    return n;
}

// if handling insertion of leaf node, calculate hv when calling function, call with n = NULL
// else feed in LHV of non leaf node, and treat n as the child node of new entry to be inserted
NODE HandleOverflow(HRT ht, NODE L, NODE n, Rect r, ull h)
{

    //Entry to be inserted
    ENTRY new_entry = (ENTRY)malloc(sizeof(Entry));
    new_entry->child = n;
    new_entry->LHV = h;
    new_entry->MBR = r;

    // Here root is splitting So we are handling this case seperately to create a new root
    // Step I4 of the insertion algorithm : If the root has been split, create a new root to enclose split nodes
    if (L->parent == NULL)
    {
        //printf("inside root splitting\n");
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
        if(j == 4) s[k] = new_entry;
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
        L->all_entries[3] = NULL;
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
        // Step H1: s is a set of entries whose children are the sibling nodes of the leaf node
        ENTRY *s = L->parent->all_entries;
        int no_of_entries = 1; // node to be inserted;
        int no_of_nodes = 0;


        for (int i = 0; i < 4; i++)
        {
            if(s[i] != NULL && s[i]->child!=NULL) no_of_entries += count_entries(s[i]->child);
        }

        for (int i = 0; i < 4; i++)
        {
            no_of_nodes += (s[i] != NULL);
        }

        // Step H2: new_entry is added to the existing set of entries from sibling nodes
        ENTRY *e_arr = (ENTRY *)malloc(sizeof(ENTRY) * no_of_entries);

        int i = 0, j = 0, k = 0; // i - counter for e_arr pointer,
        bool new_entry_inserted = false;

        for (int i = 0; i < no_of_nodes; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (s[i]->child!=NULL && s[i]->child->all_entries[j] != NULL)
                {
                    // printf("hello %lld \n",j);
                    if (new_entry_inserted == false)
                    {
                        // printf("2\n");
                        if (new_entry->LHV <= s[i]->child->all_entries[j]->LHV)
                        {
                            e_arr[k] = new_entry;
                            // printf("1?\n");
                            new_entry_inserted = true;
                            k++;
                            j--;
                            // printf("inside handleoverflow %lld\n",j);
                            continue;
                        }
                        // printf("2\n");
                    }
                    // printf("fkjwakdf %lld\n",i);
                    e_arr[k] = s[i]->child->all_entries[j];
                    s[i]->child->all_entries[j] = NULL;
                    k++;
                }
            }
        }
        if(new_entry_inserted == false) {
            e_arr[no_of_entries - 1] = new_entry;
        }

        // Step H3: If any of the sibling nodes are not full than distribute e_arr evenly among the existing
        // sibling nodes
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
                // printf("%lld\n",k);
                s[k]->child->all_entries[j] = e_arr[i];
                if (e_arr[i]!=NULL && e_arr[i]->child != NULL)
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
        // Step H4: Otherwise create and return a new node, with all the entries in e_arr distributed 
        // evenly among the existing nodes in s + new_node
        else
        {
            // create new node
            NODE new_node = (NODE)malloc(sizeof(struct Node));
            new_node->parent = NULL;
            // evenly distribute all entries into all 5 nodes

            int av = (no_of_entries) / (no_of_nodes + 1);
            int rem = (no_of_entries) % (no_of_nodes + 1);
            int rem2 = rem;
            //printf("%lld %lld\n", av, rem);
            int i;
            int j = 0;
            int k = 0; // current entry in parent node
            for (i = 0; i < (no_of_nodes * av + rem2); i++)
            {
                s[k]->child->all_entries[j] = e_arr[i];
                if (e_arr[i]!=NULL && e_arr[i]->child != NULL)
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

            for(int i = 0; i < no_of_nodes; i++) { // NON TESTING CODE DO NOT DELETE
                s[i] = findMBR(s[i]->child);
                s[i]->child->parent = L->parent;
            }
            //printf("The old nodes are :\n");
            //for(int i = 0; i < no_of_nodes; i++) {
              //  printNode(s[i]->child);
            //}
            j = 0;
            while (i < no_of_entries)
            {
                new_node->all_entries[j] = e_arr[i];
                i++;
                j++;
            }
            
           // printf("Hello the new node is : ");
            //printNode(new_node);
            return new_node;
        }
    }
}

void AdjustTree(HRT ht, NODE S, NODE NN)
{
    // Step A1:  If reached root level, stop
    if (S == NULL)
        return;
    else
    {   
        // Step A2: Propagate node split upwards

        // Here, S = Np
        NODE PP = NULL;
        if (NN != NULL) 
        {
            ENTRY entry_to_be_inserted = findMBR(NN);
            NODE leaf = S;
            if (leaf->all_entries[3] == NULL) // If S has room, insert NN in S according to it's hilbert value
            {
                for (int i = 0; i < 4; i++)
                {
                    if (leaf->all_entries[i] != NULL && leaf->all_entries[i]->LHV < entry_to_be_inserted->LHV)
                    {
                        continue;
                    }
                    else
                    {
                        if (leaf->all_entries[i] == NULL)
                        {
                            leaf->all_entries[i] = entry_to_be_inserted;
                        }
                        else
                        {
                            ENTRY temp = (ENTRY) malloc(sizeof(Entry));
                            temp->LHV = leaf->all_entries[i]->LHV;
                            temp->MBR = leaf->all_entries[i]->MBR;
                            temp->child = leaf->all_entries[i]->child;
                            leaf->all_entries[i] = entry_to_be_inserted;
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
            }
            else {
                // If S is split, initialize PP with the new node
                PP = HandleOverflow(ht, S, NN, entry_to_be_inserted->MBR, entry_to_be_inserted->LHV);
            }
        }

        // Step A3: adjust the MBRs and LHVs of the nodes in the parent level
        for (int i = 0; i < 4; i++)
        {
            ENTRY e = S->all_entries[i];
            int minx = INT_MAX, maxx = INT_MIN, miny = INT_MAX, maxy = INT_MIN;
            ull lhv = 0;
            if (e != NULL && e->child!=NULL)
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
                new_MBR->top_right.y = maxy;
                e->MBR = *new_MBR;
                e->LHV = lhv;
                e->child->parent = S;
            }

            if(PP != NULL) {
                ENTRY e = PP->all_entries[i];
                int minx = INT_MAX, maxx = INT_MIN, miny = INT_MAX, maxy = INT_MIN;
                ull lhv = 0;
                if (e != NULL && e->child != NULL)
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
                    new_MBR->top_right.y = maxy;
                    e->MBR = *new_MBR;
                    e->LHV = lhv;
                    e->child->parent = PP;
                }
            }
        }
        // Step A4: Move up to the next level and repeat from A1
        AdjustTree(ht, S->parent, PP);
    }
}

void insert(HRT ht, Rect r)
{
    ull h = calculate_hilbert_value(r);
    if (ht->root->all_entries[0] == NULL)
    {
        ENTRY e = (ENTRY)malloc(sizeof(Entry));
        e->MBR = r;
        e->LHV = h;
        e->child = NULL;
        ht->root->all_entries[0] = e;
        return;
    }

    // Step I1 : Find the appropriate leaf node
    NODE leaf = chooseLeaf(ht, r, h);

    // Step I2 : If leaf node has room, insert new entry in order of it's hilbert value
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
                    ENTRY temp = (ENTRY) malloc(sizeof(Entry));
                    temp->LHV = leaf->all_entries[i]->LHV;
                    temp->MBR = leaf->all_entries[i]->MBR;
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
        AdjustTree(ht, leaf->parent, NULL);
    }
    // Step I2 (contd.) : Else invoke HandleOverflow to return new node if split was inevitable
    else
    {
        NODE leaf2 = HandleOverflow(ht, leaf, NULL, r, h);
        NODE Np = leaf->parent;
        
        // Step I3 : Propagate changes upward in the tree by forming S and calling AdjustTree
        // NOTE :- here, S is all of the child nodes of the entries in Np, therefore Np is passed as S
        if (leaf2 == NULL)
            AdjustTree(ht, Np, NULL);
        else
        {
            AdjustTree(ht, Np, leaf2);
        }
    }

    // Step I4 : root splitting, handled in HandleOverflow
}

void rot(int n, int *x, int *y, ull rx, ull ry)
{
    if (ry == 0)
    {
        if (rx == 1)
        {
            *x = n - 1 - *x;
            *y = n - 1 - *y;
        }

        // Swap x and y
        int t = *x;
        *x = *y;
        *y = t;
    }
}

ull xy2d(int n, int x, int y)
{
    ull rx, ry, s; ull d = 0;
    for (s = n / 2; s > 0; s /= 2)
    {
        rx = (x & s) > 0;
        ry = (y & s) > 0;
        d += s * s * ((3 * rx) ^ ry);
        rot(n, &x, &y, rx, ry);
    }
    return d;
}

ull calculate_hilbert_value(Rect r)
{
    return xy2d(hilbert_N, (r.top_right.x + r.bottom_left.x) / 2, (r.top_right.y + r.bottom_left.y) / 2);
}

bool isLeaf(NODE n)
{
    for (int i = 0; i < 4; i++)
    {
        if (n->all_entries[i]!=NULL && n->all_entries[i]->child == NULL)
        {
            continue;
        }
        else
        {
            if(n->all_entries[i]==NULL)
            break;
            else
            return false;
        }
    }
    return true;
}

void set_lhv(ENTRY e)
{ // UNTESTED
    NODE n = e->child;

    if (n == NULL)
    {
        e->LHV = calculate_hilbert_value(e->MBR);
    }
    else
    {
        int new_lhv = 0;
        for (int i = 0; i < 4; i++)
        {
            if (n->all_entries[i]->LHV > new_lhv)
            {
                new_lhv = n->all_entries[i]->LHV;
            }
        }

        e->LHV = new_lhv;
    }
}

bool intersects(Rect r, Rect w)
{
    // Check if the rectangles do not intersect
    if (r.top_right.x < w.bottom_left.x || r.bottom_left.x > w.top_right.x || r.top_right.y < w.bottom_left.y || r.bottom_left.y > w.top_right.y)
    {
        return false;
    }
    // If the rectangles intersect, return true
    return true;
}

void printRect(Rect r) {
    printf("(%d %d) (%d %d)\n", r.bottom_left.x, r.bottom_left.y, r.top_right.x, r.top_right.y);
}

void search(NODE root, Rect w, int *p)
{   
    // S2 : Report all entries at leaf node level who intersect with the target rectangle as candidates
    if(isLeaf(root)) {
        for(int i = 0; i < 4; i++) {
            if(root->all_entries[i] != NULL) {
                if(intersects(root->all_entries[i]->MBR, w)) {
                    (*p)++;
                    printRect(root->all_entries[i]->MBR);
                }
            }
        }
        return;
    }
    
    // S1 : invoke search for every entry whose MBR intersects with the query rectangle
    for (int i = 0; i < 4; i++)
    {
        if (root->all_entries[i] != NULL)
        {
            ENTRY e = root->all_entries[i];
            if(e != NULL) {
                if(intersects(e->MBR, w)) {
                    search(e->child, w, p);
                }
            }
        }
    }
}

void search_wrapper(HRT ht, Rect w) {
    int a = 0;
    int *ptr;
    ptr = &a;

    printf("For the rectangle ");
    printRect(w);
    search(ht->root, w, ptr);
    if(*ptr == 0) {
        printf("No intersecting data rectangle was found\n");
    }
    printf("\n");
}

void pre_order_traversal(NODE root, int j)
{
    if(root == NULL) {
        printf("End reached at level %lld\n", j);
        check++;
        return;
    }
    for (int i = 0; i < 4; i++)
    {
        if (root->all_entries[i] != NULL)
        {
            printf("inside ");
            ENTRY e = root->all_entries[i];
            printf("(%lld %lld), (%lld %lld), level %lld, node_no %lld\n",e->MBR.top_right.x, e->MBR.top_right.y, e->MBR.bottom_left.x, e->MBR.bottom_left.y, j, i);
            pre_order_traversal(e->child, j + 1);
        }
    }
}

Rect *createNewRect(Point p)
{
    Rect *rect = (Rect *)malloc(sizeof(Rect));
    rect->bottom_left = p;
    rect->top_right = p;
    return rect;
}

Point *createNewPoint(int x, int y)
{
    Point *point = (Point *)malloc(sizeof(Point));
    point->x = x;
    point->y = y;
    return point;
}

NODE createNewNodeOfTree()
{
    NODE n = (NODE)malloc(sizeof(struct Node));
    for (int i = 0; i < 4; i++)
    {
        n->all_entries[i] = NULL;
    }
    n->parent = NULL;
    return n;
}


int main()
{
    // Creating Hilbert Tree
    NODE n=createNewNodeOfTree();
    HRT ht = (HRT)malloc(sizeof(struct HRTree));
    ht->root = n;
    char str[100];
    printf("Enter the name of the file from which you want to get the data\n");
    scanf("%s",str);

    // Insertion of Nodes
    FILE *fp2 = fopen(str, "r");
    if (fp2 == NULL)
    {
        printf("Unable to open the file");
        exit(1);
    } else {
        printf("File was opened\n");
    }

    while(!feof(fp2)) {
        int x, y;
        fscanf(fp2, "%d %d", &x, &y);
        hilbert_N = MAX(hilbert_N, x);
        hilbert_N = MAX(hilbert_N, y);
    }
    fclose(fp2);

    FILE *fp = fopen(str, "r");
    int i = 0;
    while (!feof(fp))
    {
        i++;
        int x, y;
        fscanf(fp, "%lld %lld", &x, &y);
        Point *point = createNewPoint(x, y); //creating a new point using x and y
        Rect *rect = createNewRect(*point); //creating a new rectangle using the newly created point
        insert(ht, *rect); //inserting the rectangle inside the Hilbert R tree
        printf("Point taken is : (%lld %lld %lld)\n", x, y, calculate_hilbert_value(*rect));
    }
    pre_order_traversal(ht->root, 0);
    printf("No of data rectangles counted : %lld\n", check);
    Point p1 = {856029, 730586}, p2 = {856029, 730586};
    Rect re = {p1, p2};
    Point p3 = {1, 1};
    Rect re2 = {p3, p3};
    search_wrapper(ht, re);
    search_wrapper(ht, re2);
    fclose(fp);
    return 0;

}