// Include necessary libraries
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <vector>
// To run this file gcc -o seq Sequential_CountUniqueWords.c -lm

// Define a macro for maximum word length
#define MAX_WORD_LENGTH 100 

// Function to check if a given word is already in a given list
int isWordInList(char *word, char **wordList, int numWords) {
    // Iterate through the list of words
    for (int i = 0; i < numWords; i++) {
        // Compare each word with the given word
        if (strcmp(word, wordList[i]) == 0) {
            return 1; // Word is already in list
        }
    }
    return 0; // Word is not in list
}

// Function to read words from a file into an array, and return the array of unique words
int ReadFromFileToArray(const char *filename, int fileLength, char ***pppArray)
{
    // Open the file for reading
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		perror("Error opening file");
		return -1;
	}

	char word[MAX_WORD_LENGTH]; // Temporary variable to store individual words from file
	char **FileWordList = NULL;
	char **UniqueWordList = NULL;
	int UniqueWordListLength = 0;
	
    // Allocate memory to store all words in the file
	FileWordList = (char**)malloc(fileLength * sizeof(char *));
	
    // Read each word from the file and store it in FileWordList
	for(int i = 0; i < fileLength; i++){
		fscanf(file, "%s", word);
		FileWordList[i] = strdup(word);
	}
    
    // Allocate memory to store unique words
	UniqueWordList = (char**)malloc(fileLength * sizeof(char *));
	fclose(file);
	
    // Check for unique words
	for(int i = 0; i < fileLength; i++){
		// Convert each word to lowercase for case-insensitive comparison
		for (int j = 0; FileWordList[i][j]; j++) {
		    FileWordList[i][j] = tolower(FileWordList[i][j]);
		}
		// Check if the word is already in the list of unique words
		if (!isWordInList(FileWordList[i], UniqueWordList, UniqueWordListLength)) {
		    	UniqueWordList[UniqueWordListLength] = strdup(FileWordList[i]);
		    	UniqueWordListLength++;
		}
	}
	
    // Resize the array of unique words to its actual size
	UniqueWordList = realloc(UniqueWordList, UniqueWordListLength * sizeof(char *));
	*pppArray = UniqueWordList;

    // Free the memory allocated for the FileWordList
	for (int i = 0; i < fileLength; i++) {
		free(FileWordList[i]);
	}
	free(FileWordList);
	
	return UniqueWordListLength;
}

// Main function
int main() {
	char* filenames[3] = {"MOBY_DICK.txt", "LITTLE_WOMEN.txt", "SHAKESPEARE.txt"};
	int fileLengths[3] = {215724,195467,965465};

    // Array to store the list of unique words for each file
	char** ppWordListArray[3] = {0};
	int wordListLengthArray[3] = {0};
	int i;
	char writeFile[32] = {0};
	
	struct timespec start, end;
	double time_taken; 
	int n = 0;

	// Record the start time for performance measurement
	clock_gettime(CLOCK_MONOTONIC, &start); 
	
    // Read each file and find unique words
	for (i=0; i<3; i++){
		wordListLengthArray[i] = ReadFromFileToArray(filenames[i], fileLengths[i], &ppWordListArray[i]);
		n += wordListLengthArray[i];
	}
  
 	// Record the end time for performance measurement
	clock_gettime(CLOCK_MONOTONIC, &end); 
	time_taken = (end.tv_sec - start.tv_sec) * 1e9; 
    time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9; 
	printf("Total unique words from read files: %d. Process time(s): %lf\n", n, time_taken);
   
	// Free allocated memory
	for (i=0; i<3; i++){
		for (int j = 0; j < wordListLengthArray[i]; j++) {
			free(ppWordListArray[i][j]);
		}
		free(ppWordListArray[i]);	
	}

    return 0;
}
