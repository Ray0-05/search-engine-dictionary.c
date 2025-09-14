Overview

This program reads and parses lines from a CSV (Comma-Separated Values) file.
It supports:
1) Quoted fields (e.g., "Hello, world")
2) Escaped quotes inside fields ("" becomes " in output)
3) Unquoted fields Empty fields

The program ensures that each line is split correctly into the required number of fields.

============================================================================================
HOW TO COMPILE? 
==> type `make` in terminal (automates compilation)

============================================================================================
HOW TO RUN?
==> read runtests.txt

============================================================================================
FILES
driver.c ==) 
Entry point of the program

parser.c ==)
 Contains the CSV parsing logic (parsing headers and a whole dict struct)

dict_linkedlist.c ==)
implements a dictionary using a linked list, allowing records to be searched by key fields while collecting search statistics and supporting record storage/output.Also Frees everything that's used to build it(including linked list and nodes).

list.c ==) implements a generic singly linked list.
It provides basic operations such as creating a new list, appending items, retrieving the list size, and freeing the entire list.
The implementation is data-agnostic(polymophism): it does not assume or depend on the type of data stored in the nodes.
Instead, the caller supplies an optional free_item function when freeing the list, allowing safe deallocation of node contents regardless of their type.

record.c ==) This module defines how an address record is created and destroyed.
It provides functions to build an address_t from parsed CSV fields and to correctly free its memory.

bit.c / bit.h ==) Provides bit manipulation utilities (getBit, bit_compare).

io.c ==)
Handles program output.
save_record() writes a record to the output file in a formatted style.
print_stats() prints search statistics to standard output.

============================================================================================
MEMORY MANAGEMENT

Every field and record is dynamically allocated and freed properly.
valgrind reports 0 memory leaks when the program exits.
============================================================================================

