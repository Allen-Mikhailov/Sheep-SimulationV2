#include <stdio.h> 


struct Sheep
{
    int age;
    int gender;

    double x;
    double y;

    double hunger;
};

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
    int t;
};

struct LinkedList newList()
{
    struct LinkedList list;
    list.head = NULL;
    list.tail = NULL;
    return list;
}

void AddToList(struct LinkedList * list, void* obj)
{
    struct LinkedListNode newNode;
    newNode.obj = obj;
    newNode.previous = (*list).head;\
    newNode.next = NULL;

    if (list->head != NULL) {
        list->head->next = &newNode;
        list->head = &newNode;
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

    while(head->next != NULL)
    {
        if (head == head->next)
        {
            printf("Fck"); 
            break;
        }
        head = head->next;
        
        struct Sheep *shep = (struct Sheep *)  head->obj;

        // printf("L-%d\n", shep->age);
        c++;
    }

    return c;
}