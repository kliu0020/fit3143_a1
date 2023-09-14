#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <chrono>
#include <cctype>
#include <unordered_set>
#include <omp.h>

#define BLOOM_FILTER_SIZE 1000000
#define FILE_COUNT 3

std::bitset<BLOOM_FILTER_SIZE> bloom_filter;  // Single Bloom filter for all files
omp_lock_t lock;
std::unordered_set<std::string> exact_sets[FILE_COUNT];  // Array to store exact words for each file

/**
 * The function `hash1` calculates a hash value for a given string using the djb2 algorithm.
 * 
 * @param str The parameter `str` is a constant reference to a `std::string` object. It represents the
 * string for which we want to calculate the hash value.
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
 * @param str The parameter `str` is a constant reference to a `std::string` object. It represents the
 * string for which we want to calculate the hash value.
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
 * @param str The parameter `str` is a constant reference to a `std::string` object. It represents the
 * string for which we want to calculate the hash value.
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
 * Bloom filter, and inserts them into an unordered set if they are not.
 * 
 * @param filename The filename parameter is a string that represents the name of the file from which
 * we want to read words.
 * @param filter The parameter `filter` is a reference to a `std::bitset` object with a size of
 * `BLOOM_FILTER_SIZE`. This `std::bitset` is used as a Bloom filter to check for the presence of words
 * in the set.
 * @param exact_set The `exact_set` parameter is an `std::unordered_set<std::string>` which is used to
 * store the unique words read from the file.
 * 
 * @return the count of unique words that were inserted into the `exact_set`.
 */

int ReadAndInsert(const std::string& filename, std::bitset<BLOOM_FILTER_SIZE>& filter, std::unordered_set<std::string>& exact_set) {
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
        exact_set.insert(word);
    }

    return uniqueWordsCount;
}
/**
 * The function QueryBloomFilters takes in a query file, an array of bloom filters, an array of exact
 * sets, and the number of files, and checks for false positives in the bloom filters for each query
 * word.
 * 
 * @param query_filename The query_filename parameter is a string that represents the name of the file
 * containing the queries.
 * @param bloom_filters An array of bloom filters. Each bloom filter is represented by a
 * std::bitset<BLOOM_FILTER_SIZE> object.
 * @param exact_sets The parameter `exact_sets` is an array of `std::unordered_set<std::string>`. It is
 * used to store the exact sets of words for each file. Each element in the array corresponds to a
 * file, and the `std::unordered_set<std::string>` stores the unique words present
 * @param fileCount The parameter `fileCount` represents the number of files or bloom filters in the
 * `bloom_filters` array and the `exact_sets` array. It indicates the size of these arrays and
 * determines the number of iterations in the for loop that checks each bloom filter and exact set.
 */

void QueryBloomFilters(const std::string& query_filename, std::bitset<BLOOM_FILTER_SIZE> bloom_filters[], std::unordered_set<std::string> exact_sets[], int fileCount) {
    std::ifstream query_file(query_filename);
    std::string query_word;
    int dummy;
    int count_false_positive = 0;

    if (!query_file.is_open()) {
        std::cerr << "Failed to open query file" << std::endl;
        exit(1);
    }

    while (query_file >> query_word >> dummy) {
        for (char& c : query_word) {
            c = std::tolower(c);
        }

        bool existsInAny = false;
        bool isFalsePositive = true;

    /* checking if a query word exists in any of the Bloom filters and exact sets. */
        for (int i = 0; i < fileCount; ++i) {
            if (bloom_filters[i][hash1(query_word)] &&
                bloom_filters[i][hash2(query_word)] &&
                bloom_filters[i][hash3(query_word)]) {

                existsInAny = true;

                if (exact_sets[i].find(query_word) != exact_sets[i].end()) {
                    isFalsePositive = false;
                    break;
                }
            }
        }

        if (existsInAny && isFalsePositive) {
            count_false_positive++;
        }
    }
    std::cout << "Number of false positives: " << count_false_positive << std::endl;
}
/**
 * The main function reads multiple files, inserts unique words into bloom filters, measures the time
 * taken for each file, and outputs the total time taken and the total number of unique words.
 * 
 * @return The main function is returning an integer value of 0.
 */
int main() {
    const std::string filenames[] = {"MOBY_DICK.txt", "LITTLE_WOMEN.txt", "SHAKESPEARE.txt"};
    std::bitset<BLOOM_FILTER_SIZE> bloom_filters[FILE_COUNT];
    int uniqueWordsCount[FILE_COUNT] = {0};
    int totalUniqueWords = 0;

    auto t1 = std::chrono::high_resolution_clock::now();

    #pragma omp parallel for reduction(+:totalUniqueWords)
    /* loop that iterates over the `FILE_COUNT` number of files. For each
    file, it measures the time taken to read the file, inserts the unique words into the corresponding
    Bloom filter and exact set, and updates the total number of unique words. */
    for (int i = 0; i < FILE_COUNT; ++i) {
        auto readStart = std::chrono::high_resolution_clock::now();
        uniqueWordsCount[i] = ReadAndInsert(filenames[i], bloom_filters[i], exact_sets[i]);
        totalUniqueWords += uniqueWordsCount[i];
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

    QueryBloomFilters("query.txt", bloom_filters, exact_sets, FILE_COUNT);
    return 0;
}
