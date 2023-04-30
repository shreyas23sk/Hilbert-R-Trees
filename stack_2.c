#include "stack.h"
#include "linked_list.h"
#include <stdio.h>
#include <stdlib.h>

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



