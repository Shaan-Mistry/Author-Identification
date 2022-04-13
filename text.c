#include "text.h"
#include "parser.h"
#include "ht.h"
#include "bf.h"
#include "metric.h"

#include <stdlib.h>
#include <regex.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

#define WORD "[A-Za-z]+(['-]?[A-Za-z]+)*"

uint32_t noiselimit;

struct Text {
    HashTable *ht;
    BloomFilter *bf;
    uint32_t word_count;
};

/* Create a new Text. */
Text *text_create(FILE *infile, Text *noise) {
    Text *text = (Text *) malloc(sizeof(Text));

    if (text) {
        text->ht = ht_create(524288); // Size 2^19 = 524288
        text->bf = bf_create(2097152); // Size 2^21.
        text->word_count = 0;

        if (text->ht && text->bf) {
            regex_t re;
            if (regcomp(&re, WORD, REG_EXTENDED)) {
                fprintf(stderr, "Failed to compile regex.\n");
                return NULL;
            }
            char *word = NULL;
            while ((word = next_word(infile, &re)) != NULL) {
                //Convert word to lowercase.
                int i = 0;
                while (*(word + i)) {
                    *(word + i) = tolower(*(word + i));
                    i += 1;
                }
                if (noise && text_contains(noise, word)) {
                    continue;
                }
                if (noise == NULL && text->word_count == noiselimit) {
                    break;
                }
                ht_insert(text->ht, word);
                bf_insert(text->bf, word);
                text->word_count += 1;
            }
            regfree(&re);
            return text;
        }
    }
    return NULL;
}

/* Delete the inputted Text. */
void text_delete(Text **text) {
    if (*text) {
        ht_delete(&(*text)->ht);
        bf_delete(&(*text)->bf);
        free(*text);
        *text = NULL;
    }
}

/* Compute the distance between two texts. */
double text_dist(Text *text1, Text *text2, Metric metric) {
    double sum = 0;

    Node *n1 = NULL;
    Node *n2 = NULL;

    HashTableIterator *hti = hti_create(text1->ht);
    HashTableIterator *hti2 = hti_create(text2->ht);

    if (metric == MANHATTAN) {
        while ((n1 = ht_iter(hti)) != NULL) {
            if (text_contains(text2, n1->word)) {
                sum += fabs(text_frequency(text1, n1->word) - text_frequency(text2, n1->word));
            } else {
                sum += text_frequency(text1, n1->word);
            }
        }
        while ((n2 = ht_iter(hti2)) != NULL) {
            if (!text_contains(text1, n2->word)) {
                sum += text_frequency(text2, n2->word);
            }
        }
        hti_delete(&hti);
        hti_delete(&hti2);
        return sum;
    }
    if (metric == COSINE) {
        while ((n1 = ht_iter(hti)) != NULL) {
            if (text_contains(text2, n1->word)) {
                sum += text_frequency(text1, n1->word) * text_frequency(text2, n1->word);
            }
        }
        hti_delete(&hti);
        hti_delete(&hti2);
        return 1 - sum;
    }
    double d = 0;
    while ((n1 = ht_iter(hti)) != NULL) {
        if (text_contains(text2, n1->word)) {
            d = fabs(text_frequency(text1, n1->word) - text_frequency(text2, n1->word));
            sum += d * d;
        } else {
            d = text_frequency(text1, n1->word);
            sum += d * d;
        }
    }
    while ((n2 = ht_iter(hti2)) != NULL) {
        if (!text_contains(text1, n2->word)) {
            d = text_frequency(text2, n2->word);
            sum += d * d;
        }
    }
    hti_delete(&hti);
    hti_delete(&hti2);
    return sqrt(sum);
}

/* Returns the frequency of the word in the Text. */
double text_frequency(Text *text, char *word) {
    if (!text_contains(text, word)) {
        return 0;
    }
    return (double) ht_lookup(text->ht, word)->count / text->word_count;
}

/* Returns whether or not a word is in the inputted Text. */
bool text_contains(Text *text, char *word) {
    if (bf_probe(text->bf, word)) {
        return ht_lookup(text->ht, word);
    }
    return false;
}

/* Print out the inputted Text. */
void text_print(Text *text) {
    printf("word count: %u", text->word_count);
    ht_print(text->ht);
    bf_print(text->bf);
}
