#pragma once
#include <unordered_map>
#include <fstream>
#include <vector>

typedef struct {
    std::unordered_map<std::string,std::vector<size_t>> counts;
    std::unordered_map<char,size_t> alphabet;
} Data;

void writeMapEntry(std::ofstream& outfile, const std::pair<std::string, std::vector<size_t>>& entry);

// Function to save the Data struct to a binary file
void saveDataToFile(const std::unordered_map<std::string,std::vector<size_t>>& data, const std::string& filename,uint64_t k,double alpha,char mostFrequent);

// Function to read a single key-value pair from binary file
std::pair<std::string, std::vector<size_t>> readMapEntry(std::ifstream& infile);

// Function to read the Data struct from a binary file
std::unordered_map<std::string,std::vector<size_t>> readDataFromFile(const std::string& filename,uint64_t& k,double& alpha,char& mostFrequent);