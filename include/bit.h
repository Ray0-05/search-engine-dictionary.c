#ifndef BIT_H
#define BIT_H
#define BITS_PER_BYTE 8

int getBit(char *s, unsigned int bitIndex);
int bit_compare(char *str1, char *str2);
char *createStem(char *oldKey, unsigned int startBit, unsigned int numBits);
#endif 