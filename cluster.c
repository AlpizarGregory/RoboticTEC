#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "static_library/bridge.h"

#define MASTER 0
#define CHUNK_SIZE 800000  // 800 KB per process
#define MAX_WORDS 10000
#define MAX_WORD_LEN 32
#define TOP_N 3
#define TAG 0

typedef struct {
    char word[MAX_WORD_LEN];
    int count;
} WordCount;

void normalize(char *word) {
    for (int i = 0; word[i]; i++)
        word[i] = tolower(word[i]);
}

int find_word(WordCount *wc, int size, const char *word) {
    for (int i = 0; i < size; i++)
        if (strcmp(wc[i].word, word) == 0)
            return i;
    return -1;
}

void count_words(char *text, WordCount *wc, int *count) {
    char *token = strtok(text, " \t\n.,;:!?()[]{}\"'");
    while (token != NULL) {
        normalize(token);
        int idx = find_word(wc, *count, token);
        if (idx >= 0) {
            wc[idx].count++;
        } else if (*count < MAX_WORDS) {
            strncpy(wc[*count].word, token, MAX_WORD_LEN);
            wc[*count].count = 1;
            (*count)++;
        }
        token = strtok(NULL, " \t\n.,;:!?()[]{}\"'");
    }
}

int compare_counts(const void *a, const void *b) {
    return ((WordCount*)b)->count - ((WordCount*)a)->count;
}

void print_top_words(WordCount *wc, int count, int top_n) {
    qsort(wc, count, sizeof(WordCount), compare_counts);
    printf("Top %d words:\n", top_n);
    for (int i = 0; i < top_n && i < count; i++) {
        printf("  %s: %d\n", wc[i].word, wc[i].count);
    }
}

char* read_file(const char *filename, long *filesize_out) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("Failed to open file");
        return NULL;
    }

    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    char *buffer = malloc(size + 1);
    if (!buffer) {
        perror("Failed to allocate memory");
        fclose(fp);
        return NULL;
    }

    fread(buffer, 1, size, fp);
    buffer[size] = '\0';
    fclose(fp);

    *filesize_out = size;
    return buffer;
}

int write_top_word(WordCount *wc) {
    int open_connection;
    int close_connection;
    int result;

    char buffer[32];
    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "%s %d", wc[0].word, wc[0].count);

    // This line writes the top word to the Arduino
    result = write_to_arduino("LED_OFF");
    sleep(5);
    result = write_to_arduino("LED_ON");

    // What should actually be sent to the Arduino, hand moves
    // result = arduino_move_down();

    if (result < 0) {
        return result;
    }

    return 0;
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 2) {
        if (rank == MASTER) fprintf(stderr, "Usage: %s file.txt\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    char *full_text = NULL;
    long filesize = 0;
    char *chunk = malloc(CHUNK_SIZE);
    memset(chunk, 0, CHUNK_SIZE);

    WordCount merged[MAX_WORDS] = {0};

    if (rank == MASTER) {
        full_text = read_file(argv[1], &filesize);
        if (!full_text) {
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        long part_size = filesize / size;

        for (int i = 1; i < size; i++) {
            long offset = i * part_size;
            long len = (i == size - 1) ? (filesize - offset) : part_size;
            MPI_Send(full_text + offset, len, MPI_CHAR, i, TAG, MPI_COMM_WORLD);
        }

        strncpy(chunk, full_text, part_size);
    } else {
        MPI_Recv(chunk, CHUNK_SIZE, MPI_CHAR, MASTER, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    WordCount wc[MAX_WORDS] = {0};
    int wc_count = 0;
    count_words(chunk, wc, &wc_count);

    if (rank != MASTER) {
        qsort(wc, wc_count, sizeof(WordCount), compare_counts);
        MPI_Send(&wc[0], TOP_N * sizeof(WordCount), MPI_BYTE, MASTER, TAG, MPI_COMM_WORLD);
    } else {
        WordCount all[TOP_N * size];
        memcpy(all, wc, TOP_N * sizeof(WordCount));

        for (int i = 1; i < size; i++) {
            MPI_Recv(&all[i * TOP_N], TOP_N * sizeof(WordCount), MPI_BYTE, i, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        int merged_count = 0;

        for (int i = 0; i < TOP_N * size; i++) {
            int idx = find_word(merged, merged_count, all[i].word);
            if (idx >= 0)
                merged[idx].count += all[i].count;
            else {
                strncpy(merged[merged_count].word, all[i].word, MAX_WORD_LEN);
                merged[merged_count].count = all[i].count;
                merged_count++;
            }
        }

        print_top_words(merged, merged_count, TOP_N);
        free(full_text);
    }

    free(chunk);
    MPI_Finalize();

    if (rank == MASTER) {
        int write_result = write_top_word(merged);

        if (write_result < 0) {
            return write_result;
        }
    }

    return 0;
}
