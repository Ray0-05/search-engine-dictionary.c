#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "record.h"
#include "read.h"
#include "dict_common.h"
#include "patricia_tree_dict.h"

#define EXPECTED_ARGC 4
#define STAGE_INDEX 1
#define INPUT_IDX 2
#define OUTPUT_IDX 3

#define PATRICIA_STAGE   "2"
#define EZI_ADD_INDEX    1

int main(int argc, char *argv[]) {
    if (argc != EXPECTED_ARGC) {
        fprintf(stderr, "Usage: %s 2 <input.csv> <output.txt> < <keys>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (strcmp(argv[STAGE_INDEX], PATRICIA_STAGE) != 0) {
        fprintf(stderr, "This program runs Stage 2 only. Received stage '%s'.\n", argv[STAGE_INDEX]);
        exit(EXIT_FAILURE);
    }

    const char *inputCSV = argv[INPUT_IDX];
    const char *outputTxt = argv[OUTPUT_IDX];

    FILE *input_file = fopen(inputCSV, "r");
    FILE *output_file = fopen(outputTxt, "w");
    assert(input_file && output_file);

    /* Read header for output labels */
    char **headers = parse_header(input_file);
    assert(headers);

    int n = 0;
    struct csvRecord **dataset = readCSV(input_file, &n);

    struct ptDict *dict = ptDictNew(EZI_ADD_INDEX);

    /* Build Patricia tree */
    for (int i = 0; i < n; i++) {
        ptDictInsert(dict, dataset[i]);
    }

    /* Process queries from stdin */
    char *query = NULL;
    while ((query = getQuery(stdin)) != NULL) {
        struct queryResult *r = ptDictLookup(dict, query);
        /* Stage string passed to printing: "2" */
        printQueryResult(r, headers, stdout, output_file);
        freeQueryResult(r);
        free(query);
    }

    /* Cleanup */
    ptDictFree(dict);
    freeHeader(headers, NUM_FIELDS);
    freeCSV(dataset, n);
    fclose(input_file);
    fclose(output_file);

    return EXIT_SUCCESS;
}
