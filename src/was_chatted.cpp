#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <unordered_map>
#include <algorithm>
#include <set>

using namespace std;


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


// Aplicar ambas as classes ao modelo de contexto finito e guardar os seus resultados
// Posteriormente, aplicar cada um ao texto em analise 
// O que tiver menor valor, significa que o texto pertencerá a essa classe

unordered_map<string, vector<int>> FCM(const string& filename,const int k,const vector<char>& alphabet){

// sliding window de k 
// Fazer verificação se os caracteres lidos e o proximo é um caracter ou não (.,-....)
// Ir guardando a sequencia, o proximo simbolo e a respetiva contangem 
// abc -> (a -> 1)
//     -> (b -> 2)
// .....     
    ifstream file(filename);
    unordered_map<string, std::array<int,256>> fcm2;
    unordered_map<string, vector<int>> fcm;
    string line;

    while (getline(file, line)) {
        for (size_t i = 0; i < line.size() - k; i++) {
            string context = line.substr(i, k);
            char nextSymbol = line[i + k];
            
            // Verificar se o contexto já existe na hashtable
            if (fcm.find(context) == fcm.end()) {
                // Se não existir, adiciona-lo
                // Inicializar o array com o tamanho do alfabeto em questao
                fcm[context] = vector<int>(alphabet.size(), 0);
            }

            // Verificar se o simbolo seguinte pertence ao alfabeto e obter a respetiva posicao
            // Para incrementar o valor do array nessa posicao
            auto it = find(alphabet.begin(), alphabet.end(), nextSymbol);
            if (it != alphabet.end()) {
                int index = distance(alphabet.begin(), it);
                fcm[context][index]++;
            }
        }
    }
    return fcm;
}


// Função para determinar a que classe o texto pertence
// Com base nos resultados obtidos no modelo fcm
// Verificar a qual das classes o texto em analise pertence
string determinateClass(const unordered_map<string, vector<int>>& resultsHuman, const unordered_map<string, vector<int>>& resultsGpt, const string& textFile, const int k, const double alfa) {
    
    string line;
    double humanScore = 0.0;
    double gptScore = 0.0;
    ifstream file(textFile);
    
    // percorrer o texto em analise e para cada k simbolos, ir aplicar a probabilidade P(E|C) do proximo simbolo
    // De seguida, calculamos o numero de bits necessarios para representar esse simbolo com esse contexto (-log2(P(E|C)))
    // Incrementar uma variavel com o numero de bits totais
    // Por fim, verificar a que classe pertence
    // O que tiver menor valor significa que o texto pertence a essa classe


    // Vamos precisar dos arrays que contem as posicoes dos simbolos nas respetivas classes de texto
    // Passa los como argumentos ?

    while (getline(file, line)) {
        for (size_t i = 0; i < line.size() - k; i++) {
            string context = line.substr(i, k);

            // Verificar se o contexto existe nos resultados de ambos os modelos!!!
            
            if (resultsHuman.find(context) != resultsHuman.end()) {
               
            }

            if (resultsGpt.find(context) != resultsGpt.end()) {
              
            }
        }
    }

    if (humanScore < gptScore) {
        return "humano";
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
    
    if (argc != 4) {
        std::cerr << "Usage: "<< argv[0] << "<human_collection> <gpt_collection> <text_to_analyse>" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string humanCollectionFile = argv[1];
    std::string gptCollectionFile = argv[2];
    std::string textFile = argv[3];

    // Pode se especificar qual/quais tao incorretos
    // Verificar se os ficheiros de textos fornecidos são válidos
    if (!isValidFile(humanCollectionFile) || !isValidFile(gptCollectionFile) || !isValidFile(textFile)) {
        std::cerr << "Invalid file(s) provided!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Obter o alfabeto do ficheiro de entrada
    std::vector<char> alphabetHuman = Alphabet(humanCollectionFile);
    std::vector<char> alphabetGpt = Alphabet(gptCollectionFile);
    
    auto resultsHuman = FCM(humanCollectionFile, 3, alphabetHuman);
    auto resultsGpt = FCM(gptCollectionFile, 3, alphabetGpt);


    // Determinar a que classe o texto pertence
    string result = determinateClass(resultsHuman, resultsGpt, textFile, 3, 1);
    cout << "O texto pertence a classe: " << result << endl;


    return 0;
}