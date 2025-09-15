#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

#include "patricia_tree_dict.h"
#include "bit.h"

/* Helpers*/
static inline unsigned int keyBits(const char *key);
static struct ptNode *ptNodeNewLeaf(char *key, struct data *record);
static void ptNodeAddRecord(struct ptNode *node, struct data *record);
static int editDistance(char *str1, char *str2, int n, int m);
static int minOf3(int a, int b, int c);
static void collectDescendants(struct ptNode *node,
                               struct data ***list,
                               int *count,
                               int *cap);
static void freeNode(struct ptNode *node);


/* Node in the Patricia tree */
struct ptNode {
    char *stem;                // compressed bits (not printable as a string)
    unsigned int stemBits;     // how many bits the stem represents

    int bitIndex;              // bit position for branching (if internal node)

    struct ptNode *left;       // child when bit = 0
    struct ptNode *right;      // child when bit = 1

    // Records stored at this node (if any match exactly)
    struct data **records;     // array of pointers to data
    int recordCount;
    int recordCapacity;
};

/* Patricia tree dictionary wrapper */
struct ptDict {
    struct ptNode *root;
    int keyFieldIndex;         // which field of struct data is used as key (EZI_ADD = 1)
};

struct ptDict *ptDictNew(int keyFieldIndex) {
    assert(keyFieldIndex >= 0 && keyFieldIndex < NUM_FIELDS);
    struct ptDict *d = malloc(sizeof *d);
    assert(d);
    d->root = NULL;
    d->keyFieldIndex = keyFieldIndex;
    return d;
}

/* Helper: number of bits in a key including the null terminator */
static inline unsigned int keyBits(const char *key) {
    return (strlen(key) + 1) * BITS_PER_BYTE;
}

/* Helper: allocate a new leaf node for a record */
static struct ptNode *ptNodeNewLeaf(char *key, struct data *record) {
    struct ptNode *node = malloc(sizeof(*node));
    assert(node);

    // Store full key (treat it bit-by-bit using getBit)
    node->stem = strdup(key);
    assert(node->stem);

    // Include '\0' in bit length
    node->stemBits = keyBits(key);

    node->bitIndex = -1;         // leaf node, no branching
    node->left = NULL;
    node->right = NULL;

    // Store records
    node->records = malloc(sizeof(struct data *));
    assert(node->records);
    node->records[0] = record;
    node->recordCount = 1;
    node->recordCapacity = 1;

    return node;
}

/* Helper: Expand record array (for duplicates) */
static void ptNodeAddRecord(struct ptNode *node, struct data *record) {
    if (node->recordCount == node->recordCapacity) {
        node->recordCapacity *= 2;
        node->records = realloc(node->records,
                                node->recordCapacity * sizeof(struct data *));
        assert(node->records);
    }
    node->records[node->recordCount++] = record;
}

/* Insert a record into the Patricia tree */
void ptDictInsert(struct ptDict *dict, struct csvRecord *csvRec) {
    assert(dict && csvRec);

    // Convert csvRecord -> data (deep copy of fields)
    struct data *rec = readRecord(csvRec);
    char *key = rec->fields[dict->keyFieldIndex];
    unsigned int keyLenBits = keyBits(key);

    // Case A: empty tree
    if (!dict->root) {
        dict->root = ptNodeNewLeaf(key, rec);
        return;
    }

    struct ptNode *curr = dict->root;
    struct ptNode *parent = NULL;

    while (1) {
        // Compare bit by bit against current node’s stem
        unsigned int minBits = (keyLenBits < curr->stemBits)
                                 ? keyLenBits
                                 : curr->stemBits;

        unsigned int i;
        for (i = 0; i < minBits; i++) {
            if (getBit(key, i) != getBit(curr->stem, i)) {
                break; // mismatch
            }
        }

        if (i < curr->stemBits) {
            /* Case C: mismatch inside stem → split */

            struct ptNode *newLeaf = ptNodeNewLeaf(key, rec);

            struct ptNode *branch = malloc(sizeof(*branch));
            assert(branch);

            branch->stem = createStem(key, 0, i); // common prefix bits
            branch->stemBits = i;
            branch->bitIndex = i;
            branch->records = NULL;
            branch->recordCount = 0;
            branch->recordCapacity = 0;

            if (getBit(key, i) == 0) {
                branch->left = newLeaf;
                branch->right = curr;
            } else {
                branch->right = newLeaf;
                branch->left = curr;
            }

            if (!parent) {
                dict->root = branch;
            } else {
                if (parent->left == curr) parent->left = branch;
                else parent->right = branch;
            }
            return;
        }

        // Full stem matched
        if (!curr->left && !curr->right) {
            /* Case B: leaf node */
            if (strcmp(key, curr->stem) == 0) {
                // Exact match → append record
                ptNodeAddRecord(curr, rec);
            } else {
                // Need to split (different full keys)
                unsigned int mismatchBit = 0;
                while (getBit(key, mismatchBit) == getBit(curr->stem, mismatchBit)) {
                    mismatchBit++;
                }

                struct ptNode *newLeaf = ptNodeNewLeaf(key, rec);

                struct ptNode *branch = malloc(sizeof(*branch));
                assert(branch);

                branch->stem = createStem(key, 0, mismatchBit);
                branch->stemBits = mismatchBit;
                branch->bitIndex = mismatchBit;
                branch->records = NULL;
                branch->recordCount = 0;
                branch->recordCapacity = 0;

                if (getBit(key, mismatchBit) == 0) {
                    branch->left = newLeaf;
                    branch->right = curr;
                } else {
                    branch->right = newLeaf;
                    branch->left = curr;
                }

                if (!parent) {
                    dict->root = branch;
                } else {
                    if (parent->left == curr) parent->left = branch;
                    else parent->right = branch;
                }
            }
            return;
        }

        /* Otherwise: keep descending */
        parent = curr;
        int nextBit = getBit(key, curr->bitIndex);
        curr = (nextBit == 0) ? curr->left : curr->right;
    }
}

/* helper: Recursively collect all records under a subtree into a dynamic array */
static void collectDescendants(struct ptNode *node,
                               struct data ***list,
                               int *count,
                               int *cap) {
    if (!node) return;

    // Collect records at this node (if any)
    for (int i = 0; i < node->recordCount; i++) {
        if (*count == *cap) {
            *cap = (*cap == 0) ? 4 : (*cap * 2);
            *list = realloc(*list, (*cap) * sizeof(struct data *));
            assert(*list);
        }
        (*list)[(*count)++] = node->records[i];
    }

    // Recurse left and right
    collectDescendants(node->left, list, count, cap);
    collectDescendants(node->right, list, count, cap);
}

/* helper: have we already processed this key? */
static int hasKeySeen(const char **seen, int seenCount, const char *k) {
    for (int i = 0; i < seenCount; i++) {
        if (strcmp(seen[i], k) == 0) return 1;
    }
    return 0;
}

struct queryResult *ptDictLookup(struct ptDict *dict, char *query) {
    struct queryResult *qr = malloc(sizeof(*qr));
    assert(qr);

    qr->searchString = strdup(query);
    qr->numRecords = 0;
    qr->records = NULL;
    qr->bitCount = 0;
    qr->nodeCount = 0;
    qr->stringCount = 0;

    if (!dict->root) {
        return qr;  // empty tree
    }

    struct ptNode *curr = dict->root;
    unsigned int offset = 0;  // number of prefix bits already checked

    while (curr) {
        qr->nodeCount++;

        // Compare only the *new* portion of this node's stem (beyond offset)
        unsigned int queryBits = keyBits(query);
        unsigned int newBits = (curr->stemBits > offset) ? (curr->stemBits - offset) : 0;
        unsigned int minBits = (queryBits - offset < newBits) ? (queryBits - offset) : newBits;

        unsigned int i;
        for (i = 0; i < minBits; i++) {
            qr->bitCount++;
            if (getBit((char*)query, offset + i) != getBit(curr->stem, offset + i)) {
                break; // mismatch inside this segment
            }
        }

        /* -------- internal mismatch -------- */
        if (i < newBits) {
            struct data **candidates = NULL;
            int count = 0, cap = 0;
            collectDescendants(curr, &candidates, &count, &cap);

            // Pass 1: evaluate each DISTINCT key exactly once
            int bestDist = INT_MAX;
            const char *bestKey = NULL;

            const char **seenKeys = NULL;
            int seenCount = 0, seenCap = 0;

            for (int k = 0; k < count; k++) {
                const char *candKey = candidates[k]->fields[dict->keyFieldIndex];
                if (hasKeySeen(seenKeys, seenCount, candKey)) {
                    continue; // already counted this key
                }
                // mark as seen
                if (seenCount == seenCap) {
                    seenCap = (seenCap == 0) ? 8 : (seenCap * 2);
                    seenKeys = realloc(seenKeys, seenCap * sizeof(*seenKeys));
                    assert(seenKeys);
                }
                seenKeys[seenCount++] = candKey;

                // one string comparison per DISTINCT key
                qr->stringCount++;
                int dist = editDistance((char*)query, (char*)candKey,
                                        (int)strlen(query),
                                        (int)strlen(candKey));
                if (dist < bestDist ||
                    (dist == bestDist && (!bestKey || strcmp(candKey, bestKey) < 0))) {
                    bestDist = dist;
                    bestKey = candKey;
                }
            }

            // Pass 2: collect *all* records whose key == bestKey (preserve order)
            for (int k = 0; k < count; k++) {
                const char *candKey = candidates[k]->fields[dict->keyFieldIndex];
                if (bestKey && strcmp(candKey, bestKey) == 0) {
                    qr->records = realloc(qr->records,
                                          (qr->numRecords + 1) * sizeof(*qr->records));
                    assert(qr->records);
                    qr->records[qr->numRecords++] = candidates[k];
                }
            }

            free(seenKeys);
            free(candidates);
            return qr;
        }

        offset += i; // fully matched this node's stem

        /* -------- reached a leaf -------- */
        if (!curr->left && !curr->right) {
            if (strcmp(query, curr->stem) == 0) {
                // exact match: 1 string comparison
                qr->stringCount++;
                qr->numRecords = curr->recordCount;
                qr->records = malloc(qr->numRecords * sizeof(*qr->records));
                assert(qr->records);
                memcpy(qr->records, curr->records,
                       qr->numRecords * sizeof(*qr->records));
            } else {
                // leaf mismatch: only one DISTINCT key at this leaf
                qr->stringCount++;  // count one comparison for this candidate key
            
                // return all records at this leaf (same key)
                qr->numRecords = curr->recordCount;
                qr->records = malloc(qr->numRecords * sizeof(*qr->records));
                assert(qr->records);
                for (int k = 0; k < curr->recordCount; k++) {
                    qr->records[k] = curr->records[k];
                }
            }
            return qr;
        }

        /* -------- descend to child decided by branching bit -------- */
        int nextBit = getBit((char*)query, curr->bitIndex);
        curr = (nextBit == 0) ? curr->left : curr->right;
    }

    return qr;
}




/* Returns min of 3 integers 
    reference: https://www.geeksforgeeks.org/edit-distance-in-c/ */
static int minOf3(int a, int b, int c) {
    if (a < b) {
        if(a < c) {
            return a;
        } else {
            return c;
        }
    } else {
        if(b < c) {
            return b;
        } else {
            return c;
        }
    }
}

/* Returns the edit distance of two strings
    reference: https://www.geeksforgeeks.org/edit-distance-in-c/ */
static int editDistance(char *str1, char *str2, int n, int m){
    assert(m >= 0 && n >= 0 && (str1 || m == 0) && (str2 || n == 0));
    // Declare a 2D array to store the dynamic programming
    // table
    int dp[n + 1][m + 1];

    // Initialize the dp table
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= m; j++) {
            // If the first string is empty, the only option
            // is to insert all characters of the second
            // string
            if (i == 0) {
                dp[i][j] = j;
            }
            // If the second string is empty, the only
            // option is to remove all characters of the
            // first string
            else if (j == 0) {
                dp[i][j] = i;
            }
            // If the last characters are the same, no
            // modification is necessary to the string.
            else if (str1[i - 1] == str2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            }
            // If the last characters are different,
            // consider all three operations and find the
            // minimum
            else {
                dp[i][j] = 1 + minOf3(dp[i - 1][j], dp[i][j - 1],
                    dp[i - 1][j - 1]);
            }
        }
    }

    // Return the result from the dynamic programming table
    return dp[n][m];
}

/* free nodes recursively */
static void freeNode(struct ptNode *node) {
    if (!node) return;
    freeNode(node->left);
    freeNode(node->right);
    if (node->records) {
        for (int i = 0; i < node->recordCount; i++) {
            freeData(node->records[i]);  // free the record itself
        }
        free(node->records);
    }
    free(node->stem);
    free(node);
}


void ptDictFree(struct ptDict *dict) {
    if (!dict) return;
    freeNode(dict->root);
    free(dict);
}
