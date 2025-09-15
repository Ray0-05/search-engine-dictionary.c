#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "patricia_tree_dict.h"

/* Minimal opaque struct so we can compile; real fields come later. */
struct ptDict {
    int keyFieldIndex;
    /* TODO: root pointer etc. */
};

struct ptDict *ptDictNew(int keyFieldIndex) {
    assert(keyFieldIndex >= 0 && keyFieldIndex < NUM_FIELDS);
    struct ptDict *d = malloc(sizeof(*d));
    assert(d);
    d->keyFieldIndex = keyFieldIndex;
    return d;
}

void ptDictInsert(struct ptDict *dict, struct csvRecord *rec) {
    (void)dict; (void)rec;
    /* TODO: implement trie insert */
}

struct queryResult *ptDictLookup(struct ptDict *dict, const char *query) {
    (void)dict; (void)query;
    /* return empty NOTFOUND result for now so the pipeline works */
    struct queryResult *qr = malloc(sizeof(*qr));
    assert(qr);
    qr->searchString = strdup(query ? query : "");
    qr->numRecords = 0;
    qr->records = NULL;
    qr->bitCount = 0;
    qr->nodeCount = 0;
    qr->stringCount = 0;
    return qr;
}

void ptDictFree(struct ptDict *dict) {
    if (!dict) return;
    /* TODO: free trie nodes later */
    free(dict);
}
