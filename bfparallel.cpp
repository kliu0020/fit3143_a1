#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <chrono>
#include <cctype>
#include <omp.h>

#define BLOOM_FILTER_SIZE 1000000
#define FILE_COUNT 3

std::bitset<BLOOM_FILTER_SIZE> bloom_filter;  // Single Bloom filter for all files
omp_lock_t lock;

/**
 * The function `hash1` calculates a hash value for a given string using the djb2 algorithm.
 * 
 * @param str The parameter `str` is a constant reference to a `std::string` object. 
 * 
 * @return an unsigned integer, which is the hash value of the input string.
 */
unsigned int hash1(const std::string& str) {
    unsigned int hash = 5381;
    for (char c : str) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % BLOOM_FILTER_SIZE;
}

/**
 * The function `hash2` calculates a hash value for a given string using a bitwise operation and
 * returns the hash modulo a constant value `BLOOM_FILTER_SIZE`.
 * 
 * @param str The parameter `str` is a constant reference to a `std::string` object. 
 * 
 * @return an unsigned integer, which is the hash value of the input string.
 */
unsigned int hash2(const std::string& str) {
    unsigned int hash = 0;
    for (char c : str) {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }
    return hash % BLOOM_FILTER_SIZE;
}

/**
 * The hash3 function takes a string as input and returns an unsigned integer hash value.
 * 
 * @param str The parameter `str` is a constant reference to a `std::string` object. 
 * 
 * @return an unsigned integer, which is the hash value of the input string.
 */
unsigned int hash3(const std::string& str) {
    unsigned int hash = 0;
    for (char c : str) {
        hash = c + (hash << 7) + (hash << 15) - hash;
    }
    return hash % BLOOM_FILTER_SIZE;
}
/**
 * The function reads words from a file, converts them to lowercase, checks if they are already in a
 * Bloom filter, and inserts them if they are not.
 * 
 * @param filename The filename parameter is a string that represents the name of the file from which
 * we want to read words.
 * @param filter The `filter` parameter is a reference to a `std::bitset` object with a size of
 * `BLOOM_FILTER_SIZE`. The `std::bitset` is used as a Bloom filter to check for the presence of words
 * in a file.
 * 
 * @return the number of unique words that were read from the file and inserted into the bloom filter.
 */

int ReadFromFileAndInsert(const std::string& filename, std::bitset<BLOOM_FILTER_SIZE>& filter) {
    int uniqueWordsCount = 0;
    std::ifstream file(filename);
    std::string word;

    if (!file.is_open()) {
        std::cerr << "Failed to open file" << std::endl;
        exit(1);
    }

    while (file >> word) {
        for (char& c : word) {
            c = std::tolower(c);
        }

        if (filter[hash1(word)] &&
            filter[hash2(word)] &&
            filter[hash3(word)]) {
            continue;
        }

        filter[hash1(word)] = 1;
        filter[hash2(word)] = 1;
        filter[hash3(word)] = 1;

        uniqueWordsCount++;
    }

    return uniqueWordsCount;
}
int main() {
    const std::string filenames[] = {"MOBY_DICK.txt", "LITTLE_WOMEN.txt", "SHAKESPEARE.txt"};
    std::bitset<BLOOM_FILTER_SIZE> bloom_filters[FILE_COUNT];
    int uniqueWordsCount[FILE_COUNT] = {0};

    int totalUniqueWords = 0;  // Declare the variable here

    auto t1 = std::chrono::high_resolution_clock::now();

    #pragma omp parallel for reduction(+:totalUniqueWords)
    for (int i = 0; i < FILE_COUNT; ++i) {
        auto readStart = std::chrono::high_resolution_clock::now();
        uniqueWordsCount[i] = ReadFromFileAndInsert(filenames[i], bloom_filters[i]);
        totalUniqueWords += uniqueWordsCount[i];  // This is where the reduction will take place
        auto readEnd = std::chrono::high_resolution_clock::now();

        auto readDuration = std::chrono::duration_cast<std::chrono::microseconds>(readEnd - readStart).count();
        #pragma omp critical
        {
            std::cout << "Time taken to read " << filenames[i] << ": " << readDuration << " microseconds, or approximately " << readDuration / 1000.0 << " milliseconds.\n";
        }
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

    std::cout << "Total time taken: " << duration << " microseconds, or approximately " << duration / 1000.0 << " milliseconds.\n";
    std::cout << "Total unique words from read files: " << totalUniqueWords << std::endl;

    return 0;
}