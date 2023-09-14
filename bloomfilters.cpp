#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <chrono>
#include <cctype>

#define BLOOM_FILTER_SIZE 1000000

std::bitset<BLOOM_FILTER_SIZE> bloom_filter;

unsigned int hash1(const std::string& str) {
    unsigned int hash = 5381;
    for (char c : str) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % BLOOM_FILTER_SIZE;
}

unsigned int hash2(const std::string& str) {
    unsigned int hash = 0;
    for (char c : str) {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }
    return hash % BLOOM_FILTER_SIZE;
}

unsigned int hash3(const std::string& str) {
    unsigned int hash = 0;
    for (char c : str) {
        hash = c + (hash << 7) + (hash << 15) - hash;
    }
    return hash % BLOOM_FILTER_SIZE;
}

int ReadFromFileAndInsert(const std::string& filename) {
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
        
        if (bloom_filter[hash1(word)] &&
            bloom_filter[hash2(word)] &&
            bloom_filter[hash3(word)]) {
            continue;
        }

        bloom_filter[hash1(word)] = 1;
        bloom_filter[hash2(word)] = 1;
        bloom_filter[hash3(word)] = 1;
        
        uniqueWordsCount++;
    }

    return uniqueWordsCount;
}

int main() {
    const std::string filenames[] = {"MOBY_DICK.txt", "LITTLE_WOMEN.txt", "SHAKESPEARE.txt"};
    int uniqueWordsCount = 0;
    std::chrono::high_resolution_clock::time_point t1, t2;

    // Measure total time
    t1 = std::chrono::high_resolution_clock::now();

    for (const auto& filename : filenames) {
        // Measure time taken to read each file
        auto readStart = std::chrono::high_resolution_clock::now();
        int count = ReadFromFileAndInsert(filename);
        auto readEnd = std::chrono::high_resolution_clock::now();

        auto readDuration = std::chrono::duration_cast<std::chrono::microseconds>(readEnd - readStart).count();
        std::cout << "Time taken to read " << filename << ": " << readDuration << " microseconds, or approximately " << readDuration / 1000.0 << " milliseconds.\n";

        uniqueWordsCount += count;
    }

    // Measure time taken for insertion
    auto insertEnd = std::chrono::high_resolution_clock::now();
    auto insertDuration = std::chrono::duration_cast<std::chrono::microseconds>(insertEnd - t1).count();
    std::cout << "Time taken for insertion: " << insertDuration << " microseconds, or approximately " << insertDuration / 1000.0 << " milliseconds.\n";

    // Measure total time
    t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    std::cout << "Total time taken: " << duration << " microseconds, or approximately " << duration / 1000.0 << " milliseconds.\n";

    std::cout << "Total unique words from read files: " << uniqueWordsCount << std::endl;

    return 0;
}
