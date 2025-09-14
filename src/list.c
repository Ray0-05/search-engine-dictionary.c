#include <stdlib.h>
#include <assert.h>
#include "list.h"


/* creates an empty list (return null if failed) */
list_t *list_create(void) {
    list_t *list = (list_t *)malloc(sizeof *list);
    if (!list) return NULL;
    list->head = list->tail = NULL;
    list->size = 0;
    return list;
}

/* append a record to a linked list */
void list_append(list_t *list, void *item) {
    node_t *n = (node_t *)malloc(sizeof *n);
    assert (n != NULL);
    n->item = item;
    n->next = NULL;

    /* check if it's the first node in list, head = tail = new node if yes */
    if (!list->tail){
        list->head = list->tail = n;
    } 
    else {
        list->tail->next = n; list->tail = n; 
    }
    list->size++;
}

/* return the size of the linked list */
size_t list_size(const list_t *list) {
    return list ? list->size : 0;
}

/* free the entire linked list, 2 arguments: 1) list to be freed 
    2) data-specific function to free the data in the node */
void list_free(list_t *list, void (*free_item)(void *item)) {
    if (!list) return;
    node_t *curr = list->head;
    while (curr != NULL) {
        node_t *n = curr->next;
        if (free_item){
            // free the data stored in each node 
            free_item(curr->item);
        } 
        free(curr);
        curr = n;
    }
    free(list);
}

