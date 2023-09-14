#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <bitset>

#define MAX_WORD_LENGTH 100
#define BLOOM_FILTER_SIZE 1000000

std::bitset<BLOOM_FILTER_SIZE> bloom_filter;

unsigned int hash1(const char *str) {
    unsigned int hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % BLOOM_FILTER_SIZE;
}

unsigned int hash2(const char *str) {
    unsigned int hash = 0;
    int c;
    while ((c = *str++)) {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }
    return hash % BLOOM_FILTER_SIZE;
}

unsigned int hash3(const char *str) {
    unsigned int hash = 0;
    int c;
    while ((c = *str++)) {
        hash = c + (hash << 7) + (hash << 15) - hash;
    }
    return hash % BLOOM_FILTER_SIZE;
}

int ReadFromFileToArray(const char *filename, int fileLength, char ***pppArray)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        exit(1);
    }
    char word[MAX_WORD_LENGTH];
    char **FileWordList = NULL;
    char **UniqueWordList = NULL;
    int UniqueWordListLength = 0;

    FileWordList = (char**)malloc(fileLength * sizeof(char *));
    for(int i = 0; i < fileLength; i++) {
        fscanf(file, "%s", word);
        FileWordList[i] = strdup(word);
    }



    UniqueWordList = (char**)realloc(UniqueWordList, UniqueWordListLength * sizeof(char *));
    if (UniqueWordList == NULL) {
        perror("Failed to allocate memory");
        exit(1);
    }
    *pppArray = UniqueWordList;

    for(int i = 0; i < fileLength; i++) {
        for (int j = 0; FileWordList[i][j]; j++) {
            FileWordList[i][j] = tolower(FileWordList[i][j]);
        }
        if (bloom_filter[hash1(FileWordList[i])] && 
            bloom_filter[hash2(FileWordList[i])] && 
            bloom_filter[hash3(FileWordList[i])]) {
            continue;
        }
        bloom_filter[hash1(FileWordList[i])] = 1;
        bloom_filter[hash2(FileWordList[i])] = 1;
        bloom_filter[hash3(FileWordList[i])] = 1;

        UniqueWordList[UniqueWordListLength] = strdup(FileWordList[i]);
        UniqueWordListLength++;
    }

    UniqueWordList = realloc(UniqueWordList, UniqueWordListLength * sizeof(char *));
    *pppArray = UniqueWordList;

    for (int i = 0; i < fileLength; i++) {
        free(FileWordList[i]);
    }
    free(FileWordList);
    
    return UniqueWordListLength;
}

int main() {
    struct timespec start, end, startComp, endComp; 
    const char* filenames[3] = {"MOBY_DICK.txt", "LITTLE_WOMEN.txt", "SHAKESPEARE.txt"}; // Notice the const here
    int fileLengths[3] = {215724, 195467, 965465};

    char **ppWordListArray[3] = {0};
    int wordListLengthArray[3] = {0};
    int i;

    struct timespec start, end;
    double time_taken; 
    int n = 0;

    clock_gettime(CLOCK_MONOTONIC, &start); 

    for (i = 0; i < 3; i++) {
        wordListLengthArray[i] = ReadFromFileToArray(filenames[i], fileLengths[i], &ppWordListArray[i]);
        if (wordListLengthArray[i] == 0) {
            printf("Failed to read file: %s\n", filenames[i]);
            exit(1);
        }
        n += wordListLengthArray[i];
    }


    for (int delay1 = 0; delay1 < 10000; ++delay1) {
        for (int delay2 = 0; delay2 < 10000; ++delay2) {
            volatile int dont_optimize_me = delay1 * delay2;
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end); 
    time_taken = (end.tv_sec - start.tv_sec) * 1e9; 
    time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9; 
    printf("Total unique words from read files: %d. Process time(s): %lf\n", n, time_taken);

    for (i=0; i<3; i++){
        for (int j = 0; j < wordListLengthArray[i]; j++) {
            free(ppWordListArray[i][j]);
        free(ppWordListArray[i]);    
    }

    return 0;
}
