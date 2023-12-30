#include <stdio.h>
#include <stdlib.h>

struct LinkedList
{
    struct LinkedListNode *head;
    struct LinkedListNode *tail;
    int count;
};

struct LinkedListNode
{
    struct LinkedListNode *previous;
    struct LinkedListNode *next;
    void * obj;
    int t;
};

void startList(struct LinkedList * list)
{
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

struct LinkedList * newList()
{
    struct LinkedList *list = malloc(sizeof(struct LinkedList));
    startList(list);
    return list;
}

struct LinkedListNode * AddToList(struct LinkedList * list, void* obj)
{
    struct LinkedListNode * newNode = malloc(sizeof(struct LinkedListNode));
    newNode->obj = obj;
    newNode->previous = list->head;
    newNode->next = NULL;

    list->count++;

    if (list->head != NULL) {
        list->head->next = newNode;
        list->head = newNode;
    } else {
        list->head = newNode;
        list->tail = newNode;
    }
    return newNode;
}

void RemoveFromList(struct LinkedList * list, struct LinkedListNode * node)
{
    list->count--;

    if (node->previous != NULL)
        node->previous->next = node->next;
    else
        list->tail = node->next;

    if (node->next != NULL)
        node->next->previous = node->previous;
    else
        list->head = node->previous;
}

int countList(struct LinkedList * list)
{
    struct LinkedListNode *head = list->tail;
    int c = 1;

    if (head == NULL)
        return 0;

    while(head->next != NULL)
    {
        head = head->next;
        c++;
    }

    return c;
}