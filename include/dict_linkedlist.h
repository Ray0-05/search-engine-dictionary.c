/* 
 *
 * This file implements a dictionary data structure using a linked list.
 * It provides functions to:
 *   - Build a dictionary from an existing linked list of records
 *   - Search for records by a key field, while collecting search statistics 
 *     (nodes visited, matches found, bits compared, etc.)
 *   - Print matching records to an output file in a structured format
 *   - Free the dictionary and all associated records
 *
 * The implementation relies on supporting modules for list management,
 * record handling, bit-level string comparison, and input/output utilities.
 */

#ifndef DICT_H
#define DICT_H

#include <stdio.h>
#include "list.h"   

typedef struct dict dict_t;
typedef struct search_stats search_stats_t;

/* control centre of the program, can search with any field key passed in as argument */
struct dict {
    list_t *items;       // the builded linked list to traverse
    int key_field_index;     // set which field is the key for this dictionary
};

/* for tracking the search stats summaries*/
struct search_stats {
    int nodes_count;
    int string_count;
    int bit_count;
    int matches_count;
    int is_name_printed; 
};


/* build a dictionary that adopts an existing linked list that contains data of interest */
dict_t *build_dict_from_list(list_t *list, int key_field_index);

/* search_data:
 * Search the dictionary for records whose key field matches
 * the given target string. While scanning, statistics such as number of
 * nodes examined, matches found, total bits compared, and strings compared
 * are collected into a search_stats_t struct.
 *
 * Parameters:
 *   dict        – dictionary containing a linked list of records
 *   target      – string key to search for
 *   output_file – file stream to print results to (must not be NULL)
 *   field_head  – array of header strings used when saving a record
 *
 * Returns:
 *   A heap-allocated search_stats_t pointer containing the search statistics.
 *   Caller is responsible for freeeing this struct with free().
 *
 * Side effects:
 *   Matching records are printed to output_file, with the target name printed
 *   once before the matching records.
 */
search_stats_t *search_data(dict_t *dict, char *target, FILE *output_file, char *field_head[]);

/* free dictionary, internal list, and all data*/
void  dict_list_free(dict_t *dict);

#endif 

