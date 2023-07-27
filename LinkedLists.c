#include <stdio.h> 

struct LinkedList
{
    struct LinkedListNode *head;
    struct LinkedListNode *tail;
};

struct LinkedListNode
{
    struct LinkedListNode *previous;
    struct LinkedListNode *next;
    void * obj;
};

void AddToList(struct LinkedList * list, void* obj)
{
    struct LinkedListNode newNode;
    newNode.obj = obj;
    newNode.previous = list->tail;

    if (list->head != 0) {
        list->head->next = &newNode;
    } else {
        list->head = &newNode;
        list->tail = &newNode;
    }
}

void RemoveFromList(struct LinkedListNode * node)
{
    if (node->previous != 0)
        node->previous->next = node->next;

    if (node->next != 0)
        node->next->previous = node->previous;
}

int countList(struct LinkedList * list)
{
    int c = 0;
    struct LinkedListNode *head = list->tail;

    while(head->next)
    {

    }
}