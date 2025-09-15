/*
    Linked list dictionary implementation.
    Each node stores a CSV record (struct data).
    Dictionary supports lookup by a configurable key field.

    Provides:
        - create (specify key field index)
        - insert
        - lookup by exact string on chosen key field
        - free

    This module uses common functions from dict_common.c
*/
#include "dict_common.h"
#include "linked_list_dict.h"
#include "bit.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define NUM_FIELDS 35

/* --------------------- Data Structures --------------------- */

/* Linked list node */
struct llDictNode {
    struct data *record;
    struct llDictNode *next;
};

/* Linked list dictionary */
struct llDict {
    struct llDictNode *head;
    struct llDictNode *tail;
    int keyFieldIndex;   // which field is used for lookups
};

/* --------------------- Linked List Dictionary --------------------- */

/* Create a new linked list dictionary, specify key field index */
struct llDict *llDictNew(int keyFieldIndex) {
    assert(keyFieldIndex >= 0 && keyFieldIndex < NUM_FIELDS);
    struct llDict *ret = malloc(sizeof(struct llDict));
    assert(ret);
    ret->head = NULL;
    ret->tail = NULL;
    ret->keyFieldIndex = keyFieldIndex;
    return ret;
}

/* Insert a CSV record into the linked list dictionary */
void llDictInsert(struct llDict *dict, struct csvRecord *record) {
    if (!dict) return;
    struct llDictNode *newNode = malloc(sizeof(struct llDictNode));
    assert(newNode);
    newNode->record = readRecord(record);  // from dict_common
    newNode->next = NULL;

    if (!dict->head) {
        dict->head = newNode;
        dict->tail = newNode;
    } else {
        dict->tail->next = newNode;
        dict->tail = newNode;
    }
}

/* Lookup by exact string match on the configured key field */
struct queryResult *llDictLookup(struct llDict *dict, char *query) {
    int numRecords = 0;
    struct data **records = NULL;
    int bitCount = 0, nodeCount = 0, stringCount = 0;
    int queryBitCount = (strlen(query) + 1) * BITS_PER_BYTE;

    struct llDictNode *current = dict->head;
    while (current) {
        nodeCount++;
        stringCount++;

        char *candidateKey = current->record->fields[dict->keyFieldIndex];
        int nodeBitCount = (strlen(candidateKey) + 1) * BITS_PER_BYTE;

        for (int i = 0; i <= nodeBitCount && i <= queryBitCount; i++) {
            if (i == queryBitCount && i == nodeBitCount) {
                /* Match */
                records = realloc(records, sizeof(struct data *) * (numRecords + 1));
                assert(records);
                records[numRecords++] = current->record;
                break;
            } else if (i == queryBitCount || i == nodeBitCount) {
                break; /* one string ended */
            } else {
                bitCount++;
                if (getBit(query, i) != getBit(candidateKey, i)) {
                    break;
                }
            }
        }
        current = current->next;
    }

    struct queryResult *qr = malloc(sizeof(struct queryResult));
    assert(qr);
    qr->searchString = strdup(query);
    qr->numRecords = numRecords;
    qr->records = records;
    qr->bitCount = bitCount;
    qr->nodeCount = nodeCount;
    qr->stringCount = stringCount;
    return qr;
}

/* Free entire linked list dictionary */
void llDictFree(struct llDict *dict) {
    if (!dict) return;
    struct llDictNode *current = dict->head;
    while (current) {
        struct llDictNode *next = current->next;
        freeData(current->record); // from dict_common
        free(current);
        current = next;
    }
    free(dict);
}
