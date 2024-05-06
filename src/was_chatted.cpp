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
        std::unordered_map<std::string,std::vector<size_t>> counts;
        std::shared_ptr<std::unordered_map<char,size_t>> alphabet;

        size_t symbolPosition(char symbol) const {
            return alphabet->at(symbol);
        }

    public:
        size_t n = 0;
        ModelData(std::shared_ptr<std::unordered_map<char,size_t>>& _alphabet,const double _alpha): alpha(_alpha),alphabet(_alphabet) {}
        ModelData() {}

        void increment(const std::string& context,char symbol) {
            if(counts.find(context) == counts.end()) {
                counts[context] = std::vector<size_t>(alphabet->size(),0);
            }
            counts[context][symbolPosition(symbol)]++;
        }

        virtual double estimateProbability(char symbol,const std::string& context) {
            if(counts.find(context) == counts.end()) {
                n++;
                return 1;
            }

            std::vector<size_t>& data = counts.at(context);
            size_t symbolCount = 0;
            try {
                symbolCount = data.at(symbolPosition(symbol));
            }
            catch(const std::exception& ex) { 
                std::cerr << "Symbol " << symbol << " not found in alphabet used to train the model" << std::endl;
            }

            size_t sum = accumulate(data.begin(),data.end(),0);

            return (symbolCount + alpha) / (sum + alpha * alphabet->size());
        }

        virtual void saveData(const std::string& fileName, uint64_t k) {
            Data d = {counts,*alphabet};
            saveDataToFile(d,fileName,k,alpha);
        }

        virtual void getDataFromFile(const std::string& fileName,uint64_t& k) {
            Data d = readDataFromFile(fileName,k,alpha);
            counts = d.counts;
            alphabet = std::make_shared<std::unordered_map<char,size_t>>(d.alphabet);
        }
};

class MarkovModel {
    private:
        size_t contextSize = 1;
        char mostFrequent;
        std::unordered_map<char,size_t> counts;
    protected:
        ModelData modelData;

        virtual char findMostFrequent() {
            return max_element(counts.begin(),counts.end(),[](const std::pair<char,size_t>& a,const std::pair<char,size_t>& b) {
                    return a.second < b.second;
                })->first;
        }
    public:
        virtual void train(const std::string& data) {
            for(size_t i = 0; i < data.size() - contextSize; i++) {
                std::string context = data.substr(i,contextSize);
                char symbol = data[i + contextSize];
                modelData.increment(context,symbol);
                counts[symbol]++;
            }

            mostFrequent = findMostFrequent();
        }

        virtual double calculateBits(const std::string& text) {
            std::string extraBits(contextSize,mostFrequent);
            double bits = 0;
            std::string context(contextSize,mostFrequent);
            for(size_t i = 0; i < text.size(); i++) {
                char symbol = text[i];
                double prob = modelData.estimateProbability(symbol,context);
                bits += -log2(prob);
                context = context.substr(1) + symbol;
            }

            std::cout << "number of contexts not found: " << modelData.n << std::endl;
            return bits;
        }

        void saveData(const std::string& fileName) {
            modelData.saveData(fileName,contextSize);
        }

        void getData(const std::string& fileName) {
            modelData.getDataFromFile(fileName, contextSize);
        }

        MarkovModel(const size_t _contextSize,const double _alpha,std::shared_ptr<std::unordered_map<char,size_t>>& _alphabet): contextSize(_contextSize),modelData(_alphabet,_alpha) {}
        MarkovModel(const std::string& modelDataFile) {
            getData(modelDataFile);
        }
        virtual ~MarkovModel() {}
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

void FCM(MarkovModel& model, const std::string& filename, size_t k, std::shared_ptr<std::unordered_map<char,size_t>>& alphabet) {
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
    // create file with the same name as the input file but with .csv extension
    std::string outputFilename = filename.substr(0, filename.find_last_of('.')) + ".csv";
    std::ofstream output(outputFilename);
    output << "Text,Human score,GPT score,Class" << std::endl;
    output << filename << "," << bitsHuman << "," << bitsGPT << "," << (bitsHuman < bitsGPT ? "Human" : "GPT") << std::endl;
    output.close();
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

    if(argc < 3) {
        std::cerr << "Invalid number of arguments" << std::endl;
        printTrainHelp(basename(argv[0]));
        exit(EXIT_FAILURE);
    }

    int option;
    double alpha = 1;
    uint64_t k = 10;
    std::string file(argv[2]),outputFile("output.bin");
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

    if(!isValidFile(file)) {
        exit(EXIT_FAILURE);
    }

    std::shared_ptr<std::unordered_map<char,size_t>> alphabetMap = std::make_shared<std::unordered_map<char,size_t>>(alphabet(file));
    

    MarkovModel results(k,alpha,alphabetMap);
    FCM(results,file,k,alphabetMap);
    results.saveData(outputFile);
}

void analyze(int argc,char* argv[]) {
    if (argc < 5)
    {
        std::cerr << "Invalid number of arguments" << std::endl;
        printAnalyzeHelp(basename(argv[0]));
        exit(EXIT_FAILURE);
    }

    int option;
    std::string textFile(argv[2]),humanModelData(argv[3]),gptModelData(argv[4]),output("");
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

    if(isValidFile(output))
        writeStatisticsToFile(output, humanBits, gptBits);
    else
        std::cout << ((humanBits < gptBits) ? "Human" : "GPT") << std::endl;
}

void statistics(int argc,char* argv[]) {

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

    if (argc == 1)
    {
        std::cerr << "An option must be provided" << std::endl;
        printHelp(basename(argv[0]));
        exit(EXIT_FAILURE);
    }

    // std::string option(argv[1]);
    // auto start = std::chrono::high_resolution_clock::now(); 

    // if(option == "train") {
    //     train(argc,argv);
    // }
    // else if(option == "analyze") {
    //     analyze(argc,argv);
    // }
    // else if(option == "statistics") {
    //     statistics(argc,argv);
    // }
    // else {
    //     std::cerr << "Invalid option" << std::endl;
    //     printHelp(basename(argv[0]));
    //     exit(EXIT_FAILURE);
    // }

    // auto now = std::chrono::high_resolution_clock::now();
    // auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
    // std::cout << "Elapsed time: " << elapsed << " seconds" << std::endl;

    // return EXIT_SUCCESS;

    
    if (argc != 4) {
        std::cerr << "Usage: "<< argv[0] << "<human_collection> <gpt_collection> <text_to_analyse>" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string humanCollectionFile = argv[1];
    std::string gptCollectionFile = argv[2];
    std::string textFile = argv[3];

    // Pode se especificar qual/quais tao incorretos
    // Verificar se os ficheiros de textos fornecidos são válidos
    std::cout << "Checking files..." << std::endl;
    if (!isValidFile(humanCollectionFile) || !isValidFile(gptCollectionFile) || !isValidFile(textFile)) {
        std::cerr << "Invalid file(s) provided!" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    // Obter o alfabeto do ficheiro de entrada
    std::cout << "Obtaining alphabet..." << std::endl;
    std::shared_ptr<std::unordered_map<char,size_t>> alphabetHuman = std::make_shared<std::unordered_map<char,size_t>>(alphabet(humanCollectionFile));
    std::shared_ptr<std::unordered_map<char,size_t>> alphabetGpt = std::make_shared<std::unordered_map<char,size_t>>(alphabet(gptCollectionFile));
    
    size_t k = 5;
    double alpha = 1;

    std::cout << "Applying FCM..." << std::endl;
    MarkovModel resultsHuman(k, alpha, alphabetHuman);
    MarkovModel resultsGpt(k, alpha, alphabetGpt);
    if (existsFile("./resultsHuman.bin") && existsFile("./resultsGpt.bin"))
    {
        resultsHuman.getData("./resultsHuman.bin");
        resultsGpt.getData("./resultsGpt.bin");
    }
    else
    {
        FCM(resultsHuman, humanCollectionFile, k, alphabetHuman);
        FCM(resultsGpt, gptCollectionFile, k, alphabetGpt);
        resultsHuman.saveData("./resultsHuman.bin");
        resultsGpt.saveData("./resultsGpt.bin");
    }

    std::ifstream file(textFile);
    if(!file.is_open()) {
        std::cerr << "Error opening file: " << textFile << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    const std::string text(buffer.str());

    double bitsHuman = resultsHuman.calculateBits(text);
    std::cout << "Human score: " << bitsHuman << std::endl;

    double bitsGPT = resultsGpt.calculateBits(text);
    std::cout << "GPT score: " << bitsGPT << std::endl;

    std::cout << "Determining class..." << std::endl;
    std::string res;

    if(bitsHuman < bitsGPT)
        res = "Human";
    else
        res = "GPT";

    std::cout << "The text belongs to the class: " << res << std::endl;

    writeStatisticsToFile(textFile, bitsHuman, bitsGPT);

    return 0;
}