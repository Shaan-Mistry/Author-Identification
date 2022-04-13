#include "text.h"
#include "pq.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_STRING 2048

void usage(char *exec) {
    fprintf(stderr,
        "SYNOPSIS\n"
        "   Identifies the author of some anonymous text passed through stdin.\n"
        "   Basically compute k-nearest neighbors, where k = 1 by default.\n"
        "   The number of noise words that are removed is tunable.\n"
        "   Uses either Euclidean, Manhattan, or cosine distance as a metric.\n"
        "\n"
        "USAGE\n"
        "   %s [OPTIONS]"
        "\n"
        "OPTIONS\n"
        "   -d              Database of authors and texts [default: lib.db]\n"
        "   -k matches      Set number of top matches to display [default: 5]\n"
        "   -n noisefile    Set file of words to ignore [default: noise.txt]\n"
        "   -l limit        Set noise word limit [default: 100]\n"
        "   -e              Set distance metric as Euclidean distance [default]\n"
        "   -m              Set distance metric as Manhattan distance\n"
        "   -m              Set distance metric as cosine distance\n"
        "   -h              Display program help and usage\n",
        exec);
}

int main(int argc, char **argv) {

    FILE *database = fopen("lib.db", "r");
    FILE *noise_file = fopen("noise.txt", "r");
    uint32_t matches = 5;
    noiselimit = 100;
    Metric metric = EUCLIDEAN;

    int opt;

    // Parse command-line options.
    while ((opt = getopt(argc, argv, "d:n:k:l:emch")) != -1) {
        switch (opt) {
        case 'd':
            fclose(database);
            database = fopen(optarg, "r");
            break;
        case 'n':
            fclose(noise_file);
            noise_file = fopen(optarg, "r");
            break;
        case 'k': matches = atoi(optarg); break;
        case 'l': noiselimit = atoi(optarg); break;
        case 'e': metric = EUCLIDEAN; break;
        case 'm': metric = MANHATTAN; break;
        case 'c': metric = COSINE; break;
        case 'h': usage(argv[0]); return 0;
        default: break;
        }
    }

    // Check that the database file is valid.
    if (database == NULL) {
        fprintf(stderr, "lib.db: No such file or directory\n");
        return 1;
    }

    // Check that the noise file is valid.
    if (noise_file == NULL) {
        fprintf(stderr, "noise.txt: No such file or directory\n");
        return 1;
    }

    // Create a new noise Text.
    Text *noise = text_create(noise_file, NULL);

    // Create a new anonymous Text.
    Text *sample = text_create(stdin, noise);

    // Read the first line of the database.
    uint32_t n;
    fscanf(database, "%u\n", &n);

    // Create Priority Queue that holds n elements.
    PriorityQueue *pq = pq_create(n);

    // Read in the name of the author and the path to the author's text file.
    char author[MAX_STRING];
    char path[MAX_STRING];

    while (fgets(author, sizeof(author), database) && fgets(path, sizeof(path), database)) {

        // Remove trailing newline from author and path.
        author[strlen(author) - 1] = 0;
        path[strlen(path) - 1] = 0;

        // Open the author's file and create a new Text.
        FILE *author_file = fopen(path, "r");

        if (author_file == NULL) {
            fclose(author_file);
            continue;
        }

        Text *text = text_create(author_file, noise);

        // Compute the distance between author's Text and sample Text.
        double distance = text_dist(sample, text, metric);

        // Enqueue the author and distance into the Priority Queue.
        enqueue(pq, author, distance);

        // Close author file and delete Text.
        fclose(author_file);
        text_delete(&text);
    }

    // Print out results.
    printf("Top %u, metric: %s distance, noise limit: %u\n", matches, metric_names[metric],
        noiselimit);

    char *auth;
    double dist;
    for (uint32_t i = 1; i < matches + 1; i += 1) {
        if (dequeue(pq, &auth, &dist)) {
            printf("%u) %s [%1.15f]\n", i, auth, dist);
            free(auth);
        }
    }

    // Close files.
    fclose(database);
    fclose(noise_file);

    // Delete Texts
    text_delete(&noise);
    text_delete(&sample);

    // Delete Priority Queue.
    pq_delete(&pq);

    return 0;
}
