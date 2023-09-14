#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <omp.h>
#include <pthread.h>

#define MAX_WORD_LENGTH 100
#define NUM_THREADS 3

int isWordInList(char *word, char **wordList, int numWords) {
    for (int i = 0; i < numWords; i++) {
        if (strcmp(word, wordList[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int ReadFromFileToArray(const char *filename, int fileLength, char ***ppArray) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    char word[MAX_WORD_LENGTH];
    char **FileWordList = (char **)malloc(fileLength * sizeof(char *));
    char **UniqueWordList = (char **)malloc(fileLength * sizeof(char *));
    int UniqueWordListLength = 0;

    for (int i = 0; i < fileLength; i++) {
        fscanf(file, "%s", word);
        FileWordList[i] = strdup(word);

        for (int j = 0; FileWordList[i][j]; j++) {
            FileWordList[i][j] = tolower(FileWordList[i][j]);
        }

        if (!isWordInList(FileWordList[i], UniqueWordList, UniqueWordListLength)) {
            #pragma omp critical
            UniqueWordList[UniqueWordListLength] = strdup(FileWordList[i]);
            UniqueWordListLength++;
            UniqueWordList = realloc(UniqueWordList, UniqueWordListLength * sizeof(char *));
        }
    }

    fclose(file);

    for (int i = 0; i < fileLength; i++) {
        free(FileWordList[i]);
    }
    free(FileWordList);

    *ppArray = UniqueWordList;

    return UniqueWordListLength;
}

char *filenames[3] = {"MOBY_DICK.txt", "LITTLE_WOMEN.txt", "SHAKESPEARE.txt"};
int fileLengths[3] = {215724, 195467, 965465};
char **ppWordListArray[3] = {0};
int wordListLengthArray[3] = {0};

void *ThreadFunc(void *pArg) {
    int my_rank = *((int *)pArg);
    printf("Thread %d\n", my_rank);
    wordListLengthArray[my_rank] = ReadFromFileToArray(filenames[my_rank], fileLengths[my_rank], &ppWordListArray[my_rank]);
    return NULL;
}

int main() {
    pthread_t tid[NUM_THREADS];
    int threadNum[NUM_THREADS];
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < NUM_THREADS; i++) {
        threadNum[i] = i;
        pthread_create(&tid[i], 0, ThreadFunc, &threadNum[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(tid[i], NULL);
    }

    int n = 0;
    for (int i = 0; i < 3; i++) {
        n += wordListLengthArray[i];
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    // Compute time taken
    double time_taken = (end.tv_sec - start.tv_sec) * 1e9;
    time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
    printf("Time taken: %f seconds\n", time_taken);

    return 0;
}
