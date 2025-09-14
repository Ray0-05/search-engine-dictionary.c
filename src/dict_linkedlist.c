/* this file contains the implementation of a dictionary using a linked list */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "dict_linkedlist.h"
#include "list.h"
#include "bit.h"
#include "record.h"
#include "io.h"

#define YES 1
#define NO 0

/* build a dictionary that adopts an existing linked list that contains data of interest */
dict_t *build_dict_from_list(list_t *list, int key_field_index) {
    dict_t *dict = (dict_t*)malloc(sizeof *dict);
    if (!dict) return NULL;
    dict->items = list;                  
    dict->key_field_index = key_field_index;
    return dict;
}


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
search_stats_t *search_data(dict_t *dict, char *target, FILE *output_file, char *field_head[]) {

    assert(output_file);

    list_t *list = dict->items;
    int key_idx = dict->key_field_index;
    node_t *current = list->head;

    // track stats from 0
    search_stats_t *stats = (search_stats_t *)malloc(sizeof(*stats));
    memset(stats, 0, sizeof(*stats)); 

    int target_len = strlen(target); 

    // start searching through the linked list
    while (current != NULL) {

        stats->nodes_count++; // count the number of nodes examined
        
        // store number of bits that matched
        int matched_bits = bit_compare(((address_t *)current->item)->fields[key_idx], target);

        // if all the bits match the target and the length is same
        if (matched_bits / BITS_PER_BYTE == target_len && matched_bits % BITS_PER_BYTE == 0) {

            // if target name is not printed yet in output file
            if (stats->is_name_printed == NO) {
                fprintf(output_file, "%s\n", ((address_t *)current->item)->fields[key_idx]); 
                stats->is_name_printed = YES; // ensure only printed once
            }

            stats->matches_count++; // update the number of matches reocrd found
            save_record(output_file, current->item, field_head); // store records at output file
            stats->bit_count += BITS_PER_BYTE; // include bits of \0 if all matched
        } 
        
        // update status 
        stats->bit_count += matched_bits;
        stats->string_count++;

        // go to next node
        current = current->next;
    }


    return stats; 

}


/* free dictionary, all records, and internal list. */
void dict_list_free(dict_t *dict) {
    if (!dict) return;
    list_free(dict->items, (void(*)(void*))record_free);
    free(dict);
}


