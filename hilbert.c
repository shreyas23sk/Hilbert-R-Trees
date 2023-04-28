#include "structs.h"
#include "linked_list.h"
#include "stack.h"

// COPIED FROM WIKIPEDIA - change to avoid plag
void rot(int n, int *x, int *y, int rx, int ry)
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

int xy2d(int n, int x, int y)
{
    int rx, ry, s, d = 0;
    for (s = n / 2; s > 0; s /= 2)
    {
        rx = (x & s) > 0;
        ry = (y & s) > 0;
        d += s * s * ((3 * rx) ^ ry);
        rot(n, &x, &y, rx, ry);
    }
    return d;
}

int calculate_hilbert_value(Rect r)
{
    return xy2d(20, (r.top_right.x + r.bottom_left.x) / 2, (r.top_right.y + r.bottom_left.y) / 2);
}

bool isLeaf(NODE n)
{
    for (int i = 0; i < 4; i++)
    {
        if (n->all_entries[i]->child != NULL)
        {
            continue;
        }
        else
            return false;
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
    Rect arr[] = (Rect *)malloc(sizeof(int) * 20);
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
                        push(s, currnode->all_entries[i]);
                    }
                }
            }
        }
    }
    return arr;
}

void pre_order_traversal(NODE root)
{
    for (int i = 0; i < 4; i++)
    {
        if (root->all_entries[i] != NULL)
        {
            ENTRY e = root->all_entries[i];
            printf("%d %d\n", e->MBR.bottom_left.x, e->MBR.bottom_left.y);
            pre_order_traversal(e->child);
        }
        else
            return;
    }
}