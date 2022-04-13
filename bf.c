#include "bf.h"
#include "bv.h"
#include "salts.h"
#include "speck.h"

#include <stdlib.h>
#include <stdio.h>

struct BloomFilter {
    uint64_t primary[2];
    uint64_t secondary[2];
    uint64_t tertiary[2];
    BitVector *filter;
};

/* Construct a new Bloom Filer. */
BloomFilter *bf_create(uint32_t size) {
    BloomFilter *bf = (BloomFilter *) malloc(sizeof(BloomFilter));

    bf->primary[0] = SALT_PRIMARY_LO;
    bf->primary[1] = SALT_PRIMARY_HI;

    bf->secondary[0] = SALT_SECONDARY_LO;
    bf->secondary[1] = SALT_SECONDARY_HI;

    bf->tertiary[0] = SALT_TERTIARY_LO;
    bf->tertiary[1] = SALT_TERTIARY_HI;

    bf->filter = bv_create(size);

    return bf;
}

/* Delete the inputted Bloom Filer. */
void bf_delete(BloomFilter **bf) {
    if (*bf) {
        bv_delete(&(*bf)->filter);
        free(*bf);
        *bf = NULL;
    }
}

/* Return the size of the Bloom Filer. */
uint32_t bf_size(BloomFilter *bf) {
    return bv_length(bf->filter);
}

/* inserts the inputted word into the Bloom Filer. */
void bf_insert(BloomFilter *bf, char *word) {
    uint32_t p = hash(bf->primary, word) % bf_size(bf);
    uint32_t s = hash(bf->secondary, word) % bf_size(bf);
    uint32_t t = hash(bf->tertiary, word) % bf_size(bf);

    bv_set_bit(bf->filter, p);
    bv_set_bit(bf->filter, s);
    bv_set_bit(bf->filter, t);
}

/* Probes the Bloom Filter with the inputed word. */
bool bf_probe(BloomFilter *bf, char *word) {
    uint32_t p = hash(bf->primary, word) % bf_size(bf);
    uint32_t s = hash(bf->secondary, word) % bf_size(bf);
    uint32_t t = hash(bf->tertiary, word) % bf_size(bf);

    return bv_get_bit(bf->filter, p) && bv_get_bit(bf->filter, s) && bv_get_bit(bf->filter, t);
}

/* Print out the inputted Bloom Filter. */
void bf_print(BloomFilter *bf) {
    bv_print(bf->filter);
}
