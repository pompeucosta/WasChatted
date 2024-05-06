#include <iostream>
#include "helpers.h"


void writeMapEntry(std::ofstream& outfile, const std::pair<std::string, std::vector<size_t>>& entry) {
    // Write key size
    size_t keySize = entry.first.size();
    outfile.write(reinterpret_cast<char*>(&keySize), sizeof(keySize));

    // Write key data
    outfile.write(entry.first.data(), keySize * sizeof(char));

    // Write value size
    size_t valueSize = entry.second.size();
    outfile.write(reinterpret_cast<char*>(&valueSize), sizeof(valueSize));

    // Write value data (vector elements)
    std::vector<size_t> tmp = entry.second;
    outfile.write(reinterpret_cast<char*>(tmp.data()), valueSize * sizeof(size_t));
}

// Function to save the Data struct to a binary file
void saveDataToFile(const Data& data, const std::string& filename,uint64_t k,double alpha) {
    std::ofstream outfile(filename, std::ios::binary);

    if (!outfile.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    uint64_t mk = k;
    outfile.write(reinterpret_cast<char*>(&mk),sizeof(uint64_t));

    double a = alpha;
    outfile.write(reinterpret_cast<char*>(&a),sizeof(double));

    // Write the number of entries in counts map
    size_t numCountsEntries = data.counts.size();
    outfile.write(reinterpret_cast<char*>(&numCountsEntries), sizeof(numCountsEntries));

    // Write each entry from counts map
    for (const auto& entry : data.counts) {
        writeMapEntry(outfile, entry);
    }

    // Write the number of elements in the map
    size_t numElements = data.alphabet.size();
    outfile.write(reinterpret_cast<char*>(&numElements), sizeof(numElements));

    // Write each element (key, value) to the file
    for (const auto& [key, value] : data.alphabet) {
        char tempKey = key;
        size_t tempValue = value;
        outfile.write(reinterpret_cast<const char*>(&tempKey), sizeof(tempKey));
        outfile.write(reinterpret_cast<const char*>(&tempValue), sizeof(tempValue));
    }

    outfile.close();
}

// Function to read a single key-value pair from binary file
std::pair<std::string, std::vector<size_t>> readMapEntry(std::ifstream& infile) {
    // Read key size
    size_t keySize;
    infile.read(reinterpret_cast<char*>(&keySize), sizeof(keySize));

    // Read key data
    std::vector<char> keyData(keySize);
    infile.read(keyData.data(), keySize * sizeof(char));

    // Create key from data
    std::string key(keyData.begin(),keyData.end());

    // Read value size
    size_t valueSize;
    infile.read(reinterpret_cast<char*>(&valueSize), sizeof(valueSize));

    // Read value data (vector elements)
    std::vector<size_t> valueData(valueSize);
    infile.read(reinterpret_cast<char*>(&valueData[0]), valueSize * sizeof(size_t));

    // Return the key-value pair
    return std::make_pair(key,valueData);
}

// Function to read the Data struct from a binary file
Data readDataFromFile(const std::string& filename,uint64_t& k,double& alpha) {
    Data data;
    std::ifstream infile(filename, std::ios::binary);

    if (!infile.is_open()) {
        std::cerr << "Error opening file: " << filename;
        return data; // Return empty data struct on error
    }

    infile.read(reinterpret_cast<char *>(&k), sizeof(uint64_t));
    infile.read(reinterpret_cast<char *>(&alpha), sizeof(double));

    // Read the number of entries in counts map
    size_t numCountsEntries;
    infile.read(reinterpret_cast<char*>(&numCountsEntries), sizeof(numCountsEntries));

    // Read each entry from counts map
    for (size_t i = 0; i < numCountsEntries; i++) {
        auto entry = readMapEntry(infile);
        data.counts.insert(entry);
    }

    // Read number of elements
    size_t numElements;
    infile.read(reinterpret_cast<char*>(&numElements), sizeof(numElements));

    // Read each element (key, value) from the file
    for (size_t i = 0; i < numElements; i++) {
        char key;
        size_t value;
        infile.read(reinterpret_cast<char*>(&key), sizeof(key));
        infile.read(reinterpret_cast<char*>(&value), sizeof(value));
        data.alphabet[key] = value;
    }

    infile.close();
    return data;
}
