#include "node.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Construct a new Node. */
Node *node_create(char *word) {
    Node *n = (Node *) malloc(sizeof(Node));
    n->word = strdup(word);
    n->count = 1;

    return n;
}

/* Delete the inputted Node. */
void node_delete(Node **n) {
    if (*n) {
        free((*n)->word);
        free(*n);
        *n = NULL;
    }
}

/* Print out the inputted Node. */
void node_print(Node *n) {
    printf("word: %s, count: %u\n", n->word, n->count);
}
