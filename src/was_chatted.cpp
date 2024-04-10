#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <unordered_map>

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



// Aplicar ambas as classes ao modelo de contexto finito e guardar os seus resultados
// Posteriormente, aplicar cada um ao texto em analise 
// O que tiver menor valor, significa que o texto pertencerá a essa classe

unordered_map<string, unordered_map<char, int>> FCM(const string& filename, int k){

// sliding window de k 
// Fazer verificação se os caracteres lidos e o proximo é um caracter ou não (.,-....)
// Ir guardando a sequencia, o proximo simbolo e a respetiva contangem 
// abc -> (a -> 1)
//     -> (b -> 2)
// .....     
    ifstream file(filename);
    unordered_map<string, unordered_map<char, int>> fcm;
    string line;

    while (getline(file, line)) {
        for (size_t i = 0; i < line.size() - k; i++) {
            string context = line.substr(i, k);
            char nextSymbol = line[i + k];
            fcm[context][nextSymbol]++;
        }
    }
    
    return fcm;

}




// Função para determinar a que classe o texto pertence
// Com base nos resultados obtidos no modelo fcm
// Verificar a qual das classes o texto em analise pertence
string determinateClass(){
    return "";
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


    


    return 0;
}