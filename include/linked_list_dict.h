#ifndef LINKED_LIST_DICT_H
#define LINKED_LIST_DICT_H

#include "dict_common.h"
#include "record.h"

/* --------------------- Data Structures --------------------- */

/* Linked list node (private, not exposed to main) */
struct llDictNode;

/* Linked list dictionary */
struct llDict;

/* --------------------- Function Prototypes --------------------- */

/* Create a new linked list dictionary for a given key field */
struct llDict *llDictNew(int keyFieldIndex);

/* Insert a CSV record into the linked list dictionary */
void llDictInsert(struct llDict *dict, struct csvRecord *record);

/* Lookup by exact string match on the configured key field */
struct queryResult *llDictLookup(struct llDict *dict, char *query);

/* Free entire linked list dictionary */
void llDictFree(struct llDict *dict);

#endif
