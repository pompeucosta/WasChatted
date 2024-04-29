#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <unordered_map>
#include <algorithm>
#include <set>
#include <numeric>
#include <chrono>

using namespace std;

size_t k = 3, alpha = 1;

bool isValidFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Cannot open file: " << filename << endl;
        return false;
    }
    file.close();
    return true;
}

vector<char> Alphabet(const string& inputFilename) {
    ifstream inputFile(inputFilename);
    vector<char> alphabet;
    char symbol;

    while (inputFile.get(symbol)) {
        // Verificar se o caractere já está no alfabeto
        if (find(alphabet.begin(), alphabet.end(), symbol) == alphabet.end()) {
            // Se não estiver, adicioná-lo ao alfabeto
            alphabet.push_back(symbol);
        }
    }

    inputFile.close();
    return alphabet;
}

unordered_map<string, vector<int>> FCM(const string& filename, const int k, const vector<char>& alphabet) {
    unordered_map<string, vector<int>> model;
    ifstream file(filename);
    string line;
    string context;
    
    while (getline(file, line)) {
        line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end()); // Remove whitespace
        if (line.size() < k + 1) continue; // Skip lines shorter than k + 1
        
        // Iterate through each character in the line
        for (size_t i = 0; i <= line.size() - k - 1; ++i) {
            context = line.substr(i, k); // Extract context of length k
            char symbol = line[i + k]; // Extract the symbol after the context

            // If the context is not in the model yet, add it
            if (model.find(context) == model.end()) {
                model[context] = vector<int>(alphabet.size(), 0);
            }

            // Increment count for the symbol in this context
            size_t symbolIndex = distance(alphabet.begin(), find(alphabet.begin(), alphabet.end(), symbol));
            if (symbolIndex != alphabet.size()) { // Check if symbol exists in the alphabet
                ++model[context][symbolIndex];
            }
        }
    }
    
    file.close();
    return model;
}


// Função para determinar a que classe o texto pertence
// Com base nos resultados obtidos no modelo fcm
// Verificar a qual das classes o texto em analise pertence
string determinateClass(const unordered_map<string, vector<int>>& resultsHuman, const unordered_map<string, vector<int>>& resultsGpt, const string& textFile, const int k, const double alpha) {
    double humanScore = 0.0;
    double gptScore = 0.0;
    ifstream file(textFile);
    string line;

    while (getline(file, line)) {
        for (size_t i = 0; i <= line.size() - k - 1; i++) {
            string context = line.substr(i, k);
            char nextSymbol = line[i + k];

            // Check if context is in the maps
            if (resultsHuman.find(context) != resultsHuman.end() && resultsGpt.find(context) != resultsGpt.end()) {
                // Calculate probability for human model
                double probHuman = (resultsHuman.at(context)[nextSymbol] + alpha) / (accumulate(resultsHuman.at(context).begin(), resultsHuman.at(context).end(), 0) + alpha * resultsHuman.at(context).size());
                humanScore -= log2(probHuman);

                // Calculate probability for GPT model
                double probGpt = (resultsGpt.at(context)[nextSymbol] + alpha) / (accumulate(resultsGpt.at(context).begin(), resultsGpt.at(context).end(), 0) + alpha * resultsGpt.at(context).size());
                gptScore -= log2(probGpt);
            }
        }
    }

    file.close();

    cout << "Human score: " << humanScore << endl;
    cout << "GPT score: " << gptScore << endl;
    if (humanScore < gptScore) {
        return "Human";
    } else {
        return "GPT";
    }
}


int main(int argc,char* argv[]) {
    /*argumentos:
        1 para a coleção de textos não reescrito pelo gpt
        2 para a coleção de textos reescritos pelo gpt
        3 texto em analise
        Ate agora temos estes 3
        Futuramente:
        4 tamanho da sliding window k
        5 alfa parametro de suavização
        ....
    */
    auto start = chrono::high_resolution_clock::now(); 
    
    if (argc != 4) {
        std::cerr << "Usage: "<< argv[0] << "<human_collection> <gpt_collection> <text_to_analyse>" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string humanCollectionFile = argv[1];
    std::string gptCollectionFile = argv[2];
    std::string textFile = argv[3];

    // Pode se especificar qual/quais tao incorretos
    // Verificar se os ficheiros de textos fornecidos são válidos
    cout << "Checking files..." << endl;
    if (!isValidFile(humanCollectionFile) || !isValidFile(gptCollectionFile) || !isValidFile(textFile)) {
        std::cerr << "Invalid file(s) provided!" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    // Obter o alfabeto do ficheiro de entrada
    cout << "Obtaining alphabet..." << endl;
    std::vector<char> alphabetHuman = Alphabet(humanCollectionFile);
    std::vector<char> alphabetGpt = Alphabet(gptCollectionFile);
    
    cout << "Applying FCM..." << endl;
    unordered_map<string, vector<int>> resultsHuman = FCM(humanCollectionFile, k, alphabetHuman);
    unordered_map<string, vector<int>> resultsGpt = FCM(gptCollectionFile, k, alphabetGpt);

    cout << "Determining class..." << endl;
    // Determinar a que classe o texto pertence
    string result = determinateClass(resultsHuman, resultsGpt, textFile, k, alpha);
    cout << "The text belongs to the class: " << result << endl;

    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
    cout << "Elapsed time: " << elapsed << " seconds" << endl;

    return 0;
}