#include "structs.h"
#include "linked_list.h"
#include "stack.h"
#include <limits.h>
#define MIN(a, b) a < b ? a : b
#define MAX(a, b) a > b ? a : b

int check = 0;

LIST createNewList(){
    LIST l=(LIST)malloc(sizeof(linked_list));
    l->count=0;
    l->head=NULL;
    return l;
}

LISTNODE createNewNode(NODE data){
    LISTNODE n=(LISTNODE)malloc(sizeof(nodel));
    n->data=data;
    n->next=NULL;
    return n;
}

void insertNodeIntoList(LISTNODE node,LIST list){
    LISTNODE temp=list->head;
    node->next=temp;
    list->head=node;
    list->count++;
}

void removeFirstNode(LIST list){
    LISTNODE temp=list->head;
    list->head=temp->next;
    list->count--;
    free(temp);
}

void destroyList(LIST list){
    free(list);
}

struct Stack{
    LIST list;
};

Stack *newStack(){
    Stack *s=(Stack *)malloc(sizeof(Stack));
    LIST l=createNewList();
    s->list=l;
    return s;
}

bool push(Stack *s,NODE data){
    LISTNODE n=createNewNode(data);
    insertNodeIntoList(n,s->list);
    return true;
}

bool pop(Stack *s){
    if(s->list->count!=0){
    removeFirstNode(s->list);
    return true;
    }
    else
    return false;
}

bool isEmpty(Stack *s){
    if(s->list->count==0)
    return true;
    else 
    return false;
}

NODE top(Stack *s){
    return (s->list->head->data);
}

void freeStack(Stack *s){
    destroyList(s->list);
    free(s);
}


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
    new_MBR->top_right.y = maxy;
    e->MBR = *new_MBR;
    e->LHV = lhv;
    e->child = P;
    return e;
}

NODE chooseLeaf(HRT ht, Rect r, ull h)
{
    NODE n = ht->root;

    while (isLeaf(n) == false)
    {
        //printf("inside chooseLeaf\n");
        NODE temp = NULL;
        NODE last_non_NULL_node = NULL;
        ull min_lhv = (unsigned long long) 1e15;
        for (int i = 0; i < 4; i++)
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

    // Entry to be inserted
    ENTRY new_entry = (ENTRY)malloc(sizeof(Entry));
    new_entry->child = n;
    new_entry->LHV = h;
    new_entry->MBR = r;
    // Here root is splitting So we are handling this case seperately to create a new root
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

    /*    printf("The new reassigned root is : ");
        printNode(new_root);
        printf("And it's children are : \n");
        printChildNodes(new_root);
        printChildNodes(L);
        printChildNodes(new_node); */
        return NULL;
    }
    else
    {
        //printf("Inside non-root node splitting\n");
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

        // printf("%d, %d\n", no_of_entries, no_of_nodes);

        ENTRY *e_arr = (ENTRY *)malloc(sizeof(ENTRY) * no_of_entries);

        int i = 0, j = 0, k = 0; // i - counter for e_arr pointer,
        bool new_entry_inserted = false;

        for (int i = 0; i < no_of_nodes; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (s[i]->child!=NULL && s[i]->child->all_entries[j] != NULL)
                {
                    // printf("hello %d \n",j);
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
                            // printf("inside handleoverflow %d\n",j);
                            continue;
                        }
                        // printf("2\n");
                    }
                    // printf("fkjwakdf %d\n",i);
                    e_arr[k] = s[i]->child->all_entries[j];
                    s[i]->child->all_entries[j] = NULL;
                    k++;
                }
            }
        }
        // printf("jfgkf");
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
                // printf("%d\n",k);
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
        else
        {
            // create new node
            NODE new_node = (NODE)malloc(sizeof(struct Node));
            new_node->parent = NULL;
            // evenly distribute all entries into all 5 nodes

            int av = (no_of_entries) / (no_of_nodes + 1);
            int rem = (no_of_entries) % (no_of_nodes + 1);
            int rem2 = rem;
            //printf("%d %d\n", av, rem);
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

bool checkSplit(HRT ht, NODE leaf, Rect r, ull h)
{
    //printf("inside checksplit\n");
    //printNode(leaf);
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
        //printf("end of checksplit\n");
        AdjustTree(ht, leaf->parent, NULL);
        /* printf("Root node is :\n");
        printNode(ht->root);
        printf("Printing child nodes of root : \n");
        printChildNodes(ht->root);
        printf("Going one level below : \n");
        for(int i = 0; i < 4; i++) {
            if(ht->root->all_entries[i] != NULL && ht->root->all_entries[i]->child != NULL) {
                printChildNodes(ht->root->all_entries[i]->child);
            }
        } */
    }
    // else {
    //     return false;
    // }
    else
    {
        //printf("inside handleoverflow\n");
        NODE leaf2 = HandleOverflow(ht, leaf, NULL, r, h);
        //printf("outside handleoverflow\n");
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
        //printf("inside adjustree\n");
        NODE PP = NULL;
        if (NN != NULL)
        {
            ENTRY entry_to_be_inserted = findMBR(NN);
            NODE leaf = S;
            if (leaf->all_entries[3] == NULL)
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
               // printf("PP Overflow\n");
                PP = HandleOverflow(ht, S, NN, entry_to_be_inserted->MBR, entry_to_be_inserted->LHV);
            }
        }
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
                    new_MBR->top_right.y = maxy;
                    e->MBR = *new_MBR;
                    e->LHV = lhv;
                    e->child->parent = PP;
                }
            }
        }
        AdjustTree(ht, S->parent, PP);
    }
}

void insert(HRT ht, Rect r)
{
    int h = calculate_hilbert_value(r);
    // printf("inside insert %d\n", h);
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
    return xy2d(20, (r.top_right.x + r.bottom_left.x) / 2, (r.top_right.y + r.bottom_left.y) / 2);
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

Rect *search(HRT ht, Rect w)
{
    NODE root = ht->root;
    Stack *s = newStack();
    push(s, root);
    Rect* arr = (Rect *)malloc(sizeof(Rect) * 105819);
    int count = 0;
    while (!isEmpty(s))
    {
        NODE currnode = top(s);
        pop(s);
        if (isLeaf(currnode))
        {
            for (int i = 0; i < 4; i++)
            {
                if (currnode->all_entries[i] != NULL)
                {
                    Rect r = currnode->all_entries[i]->MBR;
                    if (intersects(r, w))
                    {
                        arr[count] = r;
                        count++;
                    }
                }
            }
        }
        else
        {
            for (int i = 0; i < 4; i++)
            {
                if (currnode->all_entries[i] != NULL)
                {
                    Rect r = currnode->all_entries[i]->MBR;
                    if (intersects(r, w))
                    {   
                        arr[count] = r; count++;
                        push(s, currnode->all_entries[i]->child);
                    }
                }
            }
        }
    }
    return arr;
}

void pre_order_traversal(NODE root, int j)
{
    if(root == NULL) {
        printf("End reached at level %d\n", j);
        check++;
        return;
    }
    for (int i = 0; i < 4; i++)
    {
        if (root->all_entries[i] != NULL)
        {
            printf("inside ");
            ENTRY e = root->all_entries[i];
            printf("(%d %d), (%d %d), level %d, node_no %d\n",e->MBR.top_right.x, e->MBR.top_right.y, e->MBR.bottom_left.x, e->MBR.bottom_left.y, j, i);
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
    FILE *fp = fopen(str, "r");
    if (fp == NULL)
    {
        printf("Unable to open the file");
        exit(1);
    } else {
        printf("File was opened\n");
    }
    while (!feof(fp))
    {
        int x, y;
        fscanf(fp, "%d %d", &x, &y);
        Point *point = createNewPoint(x, y); //creating a new point using x and y
        Rect *rect = createNewRect(*point); //creating a new rectangle using the newly created point
        insert(ht, *rect); //inserting the rectangle inside the Hilbert R tree
        printf("Last point inserted : (%d %d)\n", x, y);
    }
    pre_order_traversal(ht->root, 0);
    printf("No of data rectangles counted : %d\n", check);
    // Rect* arr = search(ht, *createNewRect(*createNewPoint(1, 3)));
    Point p1 = {856029, 730586}, p2 = {856029, 730586};
    Rect re = {p1, p2};
    Rect* arr = search(ht, re);
    for (int i=0; i<(sizeof(arr)); i++){
        printf("%d %d %d %d\n", arr[i].top_right.x, arr[i].top_right.y, arr[i].bottom_left.x, arr[i].bottom_left.y);
    }
    return 0;

}