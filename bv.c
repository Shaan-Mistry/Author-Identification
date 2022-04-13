#include "bv.h"

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

struct BitVector {
    uint32_t length;
    uint8_t *vector;
};

/* Construct a Bit Vector of the inputted length. */
BitVector *bv_create(uint32_t length) {
    BitVector *bv = (BitVector *) malloc(sizeof(BitVector));
    uint32_t bytes = length / 8 + (length % 8 != 0);

    bv->length = length;
    bv->vector = (uint8_t *) calloc(bytes, sizeof(uint8_t));

    return bv;
}

/* Delete the inputted Bit Vector. */
void bv_delete(BitVector **bv) {
    if (*bv) {
        free((*bv)->vector);
        free(*bv);
        *bv = NULL;
    }
}

/* Return the length of the inputted Bit Vector. */
uint32_t bv_length(BitVector *bv) {
    return bv->length;
}

/* Set the bit at index i to 1. */
bool bv_set_bit(BitVector *bv, uint32_t i) {
    if (i >= bv_length(bv)) {
        return false;
    }
    bv->vector[i / 8] |= 0x1 << i % 8;
    return true;
}

/* Set the bit at index i to 0. */
bool bv_clr_bit(BitVector *bv, uint32_t i) {
    if (i >= bv_length(bv)) {
        return false;
    }
    bv->vector[i / 8] &= ~(0x1 << i % 8);
    return true;
}

/* Return the bit at index i. (True = 1, False = 0) */
bool bv_get_bit(BitVector *bv, uint32_t i) {
    if (i >= bv_length(bv)) {
        return false;
    }
    return bv->vector[i / 8] >> i % 8 & 0x1;
}

/* Print out the inputted Bit Vector. */
void bv_print(BitVector *bv) {
    uint32_t bytes = bv->length / 8 + (bv->length % 8 != 0);

    printf("length: %u\n", bv->length);
    for (uint32_t i = 0; i < bytes; i += 1) {
        printf("byte %u: ", i);
        printf("0x%02" PRIx8 "\n", bv->vector[i]);
    }
}
