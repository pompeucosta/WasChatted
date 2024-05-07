#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <memory>
#include <sstream>
#include <libgen.h>
#include <unistd.h>

#include "helpers.h"

class ModelData {
    private:
        double alpha;
        const std::string a = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        std::unordered_map<std::string,std::vector<size_t>> counts;


    public:
        ModelData(const double _alpha): alpha(_alpha) {}
        ModelData() {}
        size_t symbolPosition(char symbol,bool& found) const {
            auto it = std::find(a.begin(),a.end(),symbol);
            if(it == a.end()) {
                found = false;
                return 0;
            }

            found = true;
            return std::distance(a.begin(),it);
        }

        void increment(const std::string& context,char symbol) {
            bool found = false;
            int sIndex = symbolPosition(symbol,found);

            if(!found)
                return;
            
            if(counts.find(context) == counts.end()) {
                counts[context] = std::vector<size_t>(a.size(),0);
            }
            counts[context][sIndex]++;
        }

        double estimateProbability(char symbol,const std::string& context) {
            bool found = false;
            int sIndex = symbolPosition(symbol,found);
            
            if(!found)
                return 1;

            if(counts.find(context) == counts.end()) {
                return 1.0 / a.size();
            }

            std::vector<size_t>& data = counts.at(context);
            size_t symbolCount = 0;
            try {
                symbolCount = data.at(sIndex);
            }
            catch(const std::exception& ex) { 
                std::cerr << "Symbol " << symbol << " not found in alphabet used to train the model" << std::endl;
            }

            size_t sum = accumulate(data.begin(),data.end(),0);

            return (symbolCount + alpha) / (sum + alpha * a.size());
        }

        void saveData(const std::string& fileName, uint64_t k) {
            saveDataToFile(counts,fileName,k,alpha);
        }

        void getDataFromFile(const std::string& fileName,uint64_t& k) {
            counts = readDataFromFile(fileName,k,alpha);
        }
};

class MarkovModel {
    private:
        size_t contextSize = 1;
        char mostFrequent;
        std::unordered_map<char,size_t> counts;
    protected:
        ModelData modelData;

        char findMostFrequent() {
            return max_element(counts.begin(),counts.end(),[](const std::pair<char,size_t>& a,const std::pair<char,size_t>& b) {
                    return a.second < b.second;
                })->first;
        }
    public:
        void train(const std::string& data) {
            bool found = false;
            for(size_t i = 0; i < data.size() - contextSize; i++) {
                std::string context = data.substr(i,contextSize);
                char symbol = data[i + contextSize];

                modelData.symbolPosition(symbol, found);
                if (!found)
                    continue;

                modelData.increment(context,symbol);
                counts[symbol]++;
            }

            mostFrequent = findMostFrequent();
        }

        double calculateBits(const std::string& text) {
            std::string extraBits(contextSize,mostFrequent);
            double bits = 0;
            bool found = false;
            std::string context(contextSize,mostFrequent);
            for(size_t i = 0; i < text.size(); i++) {
                char symbol = text[i];
                modelData.symbolPosition(symbol, found);
                if (!found)
                    continue;

                double prob = modelData.estimateProbability(symbol,context);
                bits += -log2(prob);
                context = context.substr(1) + symbol;
            }

            return bits;
        }

        void saveData(const std::string& fileName) {
            modelData.saveData(fileName,contextSize);
        }

        void getData(const std::string& fileName) {
            modelData.getDataFromFile(fileName, contextSize);
        }

        MarkovModel(const size_t _contextSize,const double _alpha): contextSize(_contextSize),modelData(_alpha) {}
        MarkovModel(const std::string& modelDataFile) {
            getData(modelDataFile);
        }
};

bool isValidFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return false;
    }
    file.close();
    return true;
}

bool existsFile(const std::string& filename) {
    std::ifstream file(filename);
    return file.is_open();
}

std::unordered_map<char,size_t> alphabet(const std::string& inputFilename) {
    std::ifstream inputFile(inputFilename);

    std::unordered_map<char,size_t> alphabet;

    char symbol;
    size_t counter = 0;

    while (inputFile.get(symbol)) {
        // Verificar se o caractere já está no alfabeto
        if (alphabet.find(symbol) == alphabet.end()) {
            // Se não estiver, adicioná-lo ao alfabeto
            alphabet[symbol] = counter;
            counter++;
        }
    }

    inputFile.close();
    return alphabet;
}

void FCM(MarkovModel& model, const std::string& filename, size_t k) {
    #define MAX_BUFFER_SIZE 8000

    std::ifstream file(filename);

    char* charBuffer = new char[MAX_BUFFER_SIZE];
    std::string data;
    
    while (file.readsome(charBuffer,MAX_BUFFER_SIZE)) {
        data.assign(charBuffer,file.gcount());
        model.train(data);
    }

    delete[] charBuffer;
    
    file.close();
}

// write to csv file
void writeStatisticsToFile(const std::string& filename, const double bitsHuman, const double bitsGPT) {
    std::fstream file;
    file.open("../results.csv", std::ios::in);
    bool fileExists = file.is_open();
    file.close();

    file.open("../results.csv", std::ios::out | std::ios::app);
    if (!fileExists) {
        file << "Filename,Human score,GPT score,Class\n";
    }

    file << filename << "," << bitsHuman << "," << bitsGPT << "," << (bitsHuman < bitsGPT ? "Human" : "GPT") << "\n";
    file.close();
}

void printHelp(const char* programName) {
    std::cout << "Usage: " << programName << " <command> [command options...]" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  train            Trains the model" << std::endl;
    std::cout << "  analyze          Analyzes a text file to determine the class" << std::endl;
    std::cout << "  statistics       Generates statistics of the model (accuracy,f1 score,...)" << std::endl;
    std::cout << "  -h               Display this help message" << std::endl;
}

void printTrainHelp(const char* programName) {
    std::cout << "Usage: " << programName << " train <input file> [options...]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -o               Name of the output file containing the trained data (Default: output.bin)" << std::endl;
    std::cout << "  -k <value>       Set the size of the kmer (Default: 10)" << std::endl;
    std::cout << "  -a <value>       Set the value of alpha or smoothing (Default: 1)" << std::endl;
    std::cout << "  -h               Display this help message" << std::endl;
}

void printAnalyzeHelp(const char* programName) {
    std::cout << "Usage: " << programName << " analyze <input file> <human model data file> <gpt model data file> [options...]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -o               File to output result to" << std::endl;
    std::cout << "  -h               Display this help message" << std::endl;
}

void train(int argc,char* argv[]) {
    int option;
    double alpha = 1;
    uint64_t k = 10;
    std::string file,outputFile("output.bin");
    while((option = getopt(argc,argv,"o:a:k:h")) != -1) {
        switch (option)
        {
        case 'o':
            outputFile = std::string(optarg);
            break;
        case 'a':
            try
            {
                alpha = std::stod(optarg);
                if (alpha < 0)
                {
                    std::cerr << "Error: Invalid value for -a option. Must be greater than 0" << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
            catch (const std::invalid_argument &e)
            {
                std::cerr << "Error: Invalid value for -a option. Must be a double." << std::endl;
                exit(EXIT_FAILURE);
            }
            catch (const std::out_of_range &e)
            {
                std::cerr << "Error: Value for -a option out of range." << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 'k':
            try
            {
                long x = std::stol(optarg);
                if (x <= 0)
                {
                    std::cerr << "Error: Invalid value for -k option. Must be a positive integer" << std::endl;
                    exit(EXIT_FAILURE);
                }
                k = x;
            }
            catch (const std::invalid_argument &e)
            {
                std::cerr << "Error: Invalid value for -k option. Must be an integer." << std::endl;
                exit(EXIT_FAILURE);
            }
            catch (const std::out_of_range &e)
            {
                std::cerr << "Error: Value for -k option out of range." << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        case '?':
            std::cout << "Invalid option" << std::endl;
        case 'h':
        default:
            printTrainHelp(basename(argv[0]));
            exit(EXIT_SUCCESS);
            break;
        }
    }

    int numOptionsLeft = argc - optind - 1;

    if (numOptionsLeft != 1)
    {
        std::cerr << "Please provide one input file" << std::endl;
        printTrainHelp(basename(argv[0]));
        exit(EXIT_FAILURE);
    }

    file = argv[++optind];

    if(!isValidFile(file)) {
        exit(EXIT_FAILURE);
    }

    MarkovModel results(k,alpha);
    FCM(results,file,k);
    results.saveData(outputFile);
}

void analyze(int argc,char* argv[]) {
    int option;
    std::string textFile,humanModelData,gptModelData,output("");
    while ((option = getopt(argc, argv, "o:h")) != -1)
    {
        switch (option)
        {
        case 'o':
            output = optarg;
            break;
        case '?':
            std::cout << "Invalid option" << std::endl;
        case 'h':
        default:
            printAnalyzeHelp(basename(argv[0]));
            exit(EXIT_SUCCESS);
            break;
        }
    }

    int numberOfFiles = argc - optind - 1;
    
    if(numberOfFiles != 3) {
        std::cerr << "Invalid number of files" << std::endl;
        printAnalyzeHelp(basename(argv[0]));
        exit(EXIT_FAILURE);
    }

    int i = ++optind;
    textFile = argv[i++];
    humanModelData = argv[i++];
    gptModelData = argv[i];

    if(!isValidFile(textFile) || !isValidFile(humanModelData) || !isValidFile(gptModelData)) {
        exit(EXIT_FAILURE);
    }
    
    std::ifstream file(textFile);

    MarkovModel model(humanModelData);
    std::stringstream buffer;
    buffer << file.rdbuf();
    const std::string text(buffer.str());
    double humanBits = model.calculateBits(text);
    
    MarkovModel gptModel(gptModelData);
    double gptBits = gptModel.calculateBits(text);

    if(existsFile(output))
        writeStatisticsToFile(output, humanBits, gptBits);
    else
    {
        // std::cout << "human bits: " << humanBits << std::endl;
        // std::cout << "gpt bits: " << gptBits << std::endl;
        std::cout << ((humanBits < gptBits) ? "Human" : "GPT") << std::endl;
    }
}

void statistics(int argc,char* argv[]) {

}

int main(int argc,char* argv[]) {
    if (argc == 1)
    {
        std::cerr << "An option must be provided" << std::endl;
        printHelp(basename(argv[0]));
        exit(EXIT_FAILURE);
    }

    std::string option(argv[1]);
    auto start = std::chrono::high_resolution_clock::now(); 

    if(option == "train") {
        train(argc,argv);
    }
    else if(option == "analyze") {
        analyze(argc,argv);
    }
    else if(option == "statistics") {
        statistics(argc,argv);
    }
    else if(option == "-h") {
        printHelp(basename(argv[0]));
        exit(EXIT_SUCCESS);
    }
    else {
        std::cerr << "Invalid option" << std::endl;
        printHelp(basename(argv[0]));
        exit(EXIT_FAILURE);
    }

    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
    std::cout << "Elapsed time: " << elapsed << " seconds" << std::endl;

    return EXIT_SUCCESS;
}