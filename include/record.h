#ifndef RECORD_H
#define RECORD_H

#define FIELD_NUM 35

/* contains info of each address, all fields are treatedd as strings */
typedef struct{
    char **fields;  
}address_t;

/* record_create: Build an address_t record from parsed CSV fields.
 *
 * Parameters:
 *    char **fields – array of strings from the parser, each
 *                    representing one CSV column (heap-allocated).
 *
 * Returns:
 *   A pointer to a newly created record (address_t).
 *
 * Notes:
 *   - The parser does not depend on the record structure. It always
 *     outputs arrays of strings.
 *   - This function is the abstraction boundary: if address_t is later
 *     redefined (e.g., changed from an array-of-strings to another struct),
 *     only this module and the dictionary module need to
 *     change. Parser code stays the same as well as the driver module.
 */
address_t *record_create(char **fields);

/* This function frees memory allocated to store an adress record*/
void record_free(address_t *rec);

#endif


