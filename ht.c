#include "ht.h"
#include "node.h"
#include "salts.h"
#include "speck.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct HashTable {
    uint64_t salt[2];
    uint32_t size;
    Node **slots;
};

/* Construct a new Hash Table. */
HashTable *ht_create(uint32_t size) {
    HashTable *ht = (HashTable *) malloc(sizeof(HashTable));

    ht->salt[0] = SALT_HASHTABLE_LO;
    ht->salt[1] = SALT_HASHTABLE_HI;

    ht->size = size;
    ht->slots = (Node **) calloc(size, sizeof(Node));

    return ht;
}

/* Print out the inputted Hash Table. */
void ht_print(HashTable *ht) {
    for (uint32_t i = 0; i < ht->size; i += 1) {
        if (ht->slots[i] == NULL) {
            printf("Empty Slot\n");
        } else {
            node_print(ht->slots[i]);
        }
    }
}

/* Delete the inputted Hash Table. */
void ht_delete(HashTable **ht) {
    if (*ht) {
        for (uint32_t i = 0; i < ht_size(*ht); i += 1) {
            node_delete(&(*ht)->slots[i]);
        }
        free((*ht)->slots);
        free(*ht);
        *ht = NULL;
    }
}

/* Return the size of the Hash Table. */
uint32_t ht_size(HashTable *ht) {
    return ht->size;
}

/* Returns a node in the Hash Table that contains "word" if it one exits. */
// Code Inspired from Hashing Lecture.
Node *ht_lookup(HashTable *ht, char *word) {
    uint32_t count = 0;
    uint32_t index = hash(ht->salt, word) % ht_size(ht);

    while (count < ht->size) {
        Node *n = ht->slots[index];
        if (n && strcmp(n->word, word) == 0) {
            return n;
        }
        if (n == NULL) {
            return NULL;
        }
        index = (index + 1) % ht_size(ht);
        count += 1;
    }
    return NULL;
}

/* Insert the inputted word into the Hash Table. */
/* If the word already exits, increment its count. */
// Code Inspired from Hashing Lecture.
Node *ht_insert(HashTable *ht, char *word) {
    Node *n = NULL;
    if ((n = ht_lookup(ht, word)) != NULL) {
        n->count += 1;
        return n;
    }

    uint32_t count = 0;
    uint32_t index = hash(ht->salt, word) % ht_size(ht);

    while (count < ht_size(ht)) {
        if (ht->slots[index] == NULL) {
            ht->slots[index] = node_create(word);
            return ht->slots[index];
        }
        index = (index + 1) % ht_size(ht);
        count += 1;
    }
    return NULL;
}

struct HashTableIterator {
    HashTable *table;
    uint32_t slot;
};

/* Create a new hash table iterator. */
HashTableIterator *hti_create(HashTable *ht) {
    HashTableIterator *hti = (HashTableIterator *) malloc(sizeof(HashTableIterator));
    hti->table = ht;
    hti->slot = 0;

    return hti;
}

/* Delete the inputted Hash Table iterator. */
void hti_delete(HashTableIterator **hti) {
    if (*hti) {
        free(*(hti));
        *hti = NULL;
    }
}

/* Return the next valid Node in the hash table. */
/* Return NULL if full or none exits. */
Node *ht_iter(HashTableIterator *hti) {
    static uint32_t index = 0;
    Node *n = NULL;
    uint32_t size = ht_size(hti->table);
    while (index != size) {
        if (hti->table->slots[hti->slot] == NULL) {
            index += 1;
            hti->slot = (hti->slot + 1) % size;
            continue;
        }
        n = hti->table->slots[hti->slot];
        index += 1;
        hti->slot = (hti->slot + 1) % size;
        return n;
    }
    index = 0;
    return NULL;
}
