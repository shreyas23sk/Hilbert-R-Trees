#include "linked_list.h"
#include "structs.h"
#include <stdio.h>
#include <stdlib.h>

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
