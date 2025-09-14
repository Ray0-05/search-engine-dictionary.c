/*
This module implements a generic singly linked list.
It provides operations to create a new list, append nodes, check the size, and free the list.
The list is data-independent: it does not depend on the type of data stored in each node.
When freeing the list, an optional callback function (free_item) can be provided to correctly deallocate node contents if required.
*/
#ifndef LIST_H
#define LIST_H

#include "record.h"
#include "stdio.h"

typedef struct node node_t;
typedef struct list list_t;

/* linked-list node structure */
struct node {
    void *item; // pointer to address_t, casted as void* for reusability
    node_t *next;
};


/* list state: head/tail for O(1) append, and size. */
struct list {
    node_t *head, *tail;
    size_t  size;
};


/* create an empty list. */
list_t *list_create(void);

/* append one item to the tail */
void list_append(list_t *L, void *item);

/* number of items stored. */
size_t list_size(const list_t *L);


/* free nodes and free each item via that callback. */
void list_free(list_t *L, void (*free_item)(void *item));

#endif


