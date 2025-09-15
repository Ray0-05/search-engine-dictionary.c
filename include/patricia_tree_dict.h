#ifndef PATRICIA_TREE_DICT_H
#define PATRICIA_TREE_DICT_H

#include "dict_common.h"  // brings NUM_FIELDS, data, queryResult
#include "record.h"       // brings struct csvRecord

struct ptDict;

/* Create a Patricia tree dictionary using a given key field index (use 1 for EZI_ADD). */
struct ptDict *ptDictNew(int keyFieldIndex);

/* Insert one CSV record (preserve file order for duplicates of the same key). */
void ptDictInsert(struct ptDict *dict, struct csvRecord *rec);

/* Lookup: exact match or “closest” (mismatch node + edit distance).
   Fills comparisons (bitCount/nodeCount/stringCount) inside queryResult.
*/
struct queryResult *ptDictLookup(struct ptDict *dict, const char *query);

/* Free everything in the dictt. */
void ptDictFree(struct ptDict *dict);

#endif
