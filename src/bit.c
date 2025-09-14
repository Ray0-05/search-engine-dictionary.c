// this file has all the bit manipulation functions

#include <assert.h>
#include <stdio.h>
#include "bit.h"

/* retrieves the bit at a specific index from a string of bits */
int getBit(char *s, unsigned int bitIndex){
    assert(s && bitIndex >= 0);
    unsigned int byte = bitIndex / BITS_PER_BYTE;
    unsigned int indexFromLeft = bitIndex % BITS_PER_BYTE;
    /* 
        Since we split from the highest order bit first, the bit we are interested
        will be the highest order bit, rather than a bit that occurs at the end of the
        number. 
    */
    unsigned int offset = (BITS_PER_BYTE - (indexFromLeft) - 1) % BITS_PER_BYTE;
    unsigned char byteOfInterest = s[byte];
    unsigned int offsetMask = (1 << offset);
    unsigned int maskedByte = (byteOfInterest & offsetMask);
    /*
        The masked byte will still have the bit in its original position, to return
        either 0 or 1, we need to move the bit to the lowest order bit in the number.
    */
    unsigned int bitOnly = maskedByte >> offset;
    return bitOnly;
}


/* compare two strings bit by bit, return the number of bits that are different */
int bit_compare(char *str1, char *str2) {
    
    int count = 0;

    // Ensure both strings are not NULL
    if (str1 == NULL || str2 == NULL) {
        return 0; // or handle error as needed
    }

    // Compare each bit in the strings, stop when reach the end of either string
    for (int i = 0; str1[i / BITS_PER_BYTE] != '\0' && str2[i / BITS_PER_BYTE] != '\0'; i++) {
        if (getBit(str1, i) != getBit(str2, i)) {
            count++;
            break;
        }
        count++;
    }

    return count;
}
 