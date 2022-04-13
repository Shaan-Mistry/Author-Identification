#include "pq.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Pair {
    char *author;
    double distance;
} Pair;

/* Define the Priority Queue Struct. */
struct PriorityQueue {
    uint32_t tail;
    uint32_t capacity;
    Pair **pairs;
};

/* Construct a new Author-Distance Pair. */
Pair *pair_create(char *author, double dist) {
    Pair *p = (Pair *) malloc(sizeof(Pair));
    p->author = strdup(author);
    p->distance = dist;

    return p;
}

/* Delete the inputted Author-Distance Pair. */
void pair_delete(Pair **p) {
    if (*p) {
        free((*p)->author);
        free(*p);
        *p = NULL;
    }
}

/* Construct a new Priority Queue. */
PriorityQueue *pq_create(uint32_t capacity) {
    PriorityQueue *q = (PriorityQueue *) malloc(sizeof(PriorityQueue));
    if (q) {
        q->tail = 0;
        q->capacity = capacity;
        q->pairs = (Pair **) calloc(capacity, sizeof(Pair));
        if (q->pairs) {
            return q;
        }
        free(q);
    }
    return NULL;
}

/* Delete the inputted Priority Queue. */
void pq_delete(PriorityQueue **q) {
    if (*q) {
        for (uint32_t i = 0; i < pq_size(*q); i += 1) {
            pair_delete(&(*q)->pairs[i]);
        }
        free((*q)->pairs);
        free(*q);
        *q = NULL;
    }
}

/* Check if the inputted Priority Queue is empty. */
bool pq_empty(PriorityQueue *q) {
    return q->tail == 0;
}

/* Check if the inputted Priority Queue is full. */
bool pq_full(PriorityQueue *q) {
    return q->tail == q->capacity;
}

/* Check the size of the inputted Priority Queue. */
uint32_t pq_size(PriorityQueue *q) {
    return q->tail;
}

/* Rearrange the pairs of the heap to fix the heap. */
void fix_heap(PriorityQueue *q, uint32_t i) {

    if (pq_size(q) < 2) {
        return;
    }
    uint32_t min = i;
    uint32_t left = 2 * i;
    uint32_t right = 2 * i + 1;

    /* Compare left child. */
    if (left < pq_size(q) && q->pairs[left]->distance < q->pairs[min]->distance) {
        min = left;
    }
    /* Compare right child. */
    if (right < pq_size(q) && q->pairs[right]->distance < q->pairs[min]->distance) {
        min = right;
    }
    /* Check if a smaller node was found. */
    if (min != i) {
        /* Swap min node with smaller node. */
        Pair *temp = q->pairs[i];
        q->pairs[i] = q->pairs[min];
        q->pairs[min] = temp;
        fix_heap(q, min);
    }
}

/* Find highest Priority node and place it at index 0. */
void min_heap(PriorityQueue *q, uint32_t i) {
    uint32_t parent = i / 2;

    /* Check if child node is smaller than parent node. */
    if (q->pairs[i]->distance < q->pairs[parent]->distance) {
        /* Swap child node and parent node. */
        Pair *temp = q->pairs[parent];
        q->pairs[parent] = q->pairs[i];
        q->pairs[i] = temp;

        /* Check if parent node was the root. */
        if (parent != 0) {
            min_heap(q, parent);
        }
    }
}

/* Enqueue a node into the Priority Queue. */
bool enqueue(PriorityQueue *q, char *author, double dist) {
    Pair *p = pair_create(author, dist);

    if (pq_full(q)) {
        free(p);
        return false;
    }
    q->pairs[q->tail] = p;

    if (!pq_empty(q)) {
        min_heap(q, q->tail);
    }
    q->tail += 1;
    return true;
}

/* Dequeue highest Priority node from the Priority Queue. */
bool dequeue(PriorityQueue *q, char **author, double *dist) {

    if (pq_empty(q)) {
        return false;
    }

    *author = strdup(q->pairs[0]->author);
    *dist = q->pairs[0]->distance;

    pair_delete(&q->pairs[0]);

    q->pairs[0] = q->pairs[q->tail - 1];
    fix_heap(q, 0);
    q->tail -= 1;
    return true;
}

/* Print out the Priority Queueu. */
void pq_print(PriorityQueue *q) {
    for (uint32_t i = 0; i < pq_size(q); i += 1) {
        printf("Author: %s, ", q->pairs[i]->author);
        printf("Distance: %f\n", q->pairs[i]->distance);
    }
}
