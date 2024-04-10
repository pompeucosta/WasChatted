#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;


// Função para ler um ficheiro e devolver o conteudo
string readFile(const string& filename) {
    string content;
    ifstream file(filename);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            content += line + "\n";
        }
        file.close();
    } else {
        cerr << "Unable to open file: " << filename << endl;
    }
    return content;
}


// Context finite model
//Criar 2 modelos, para cada classe e dps aplicar o texto a esses modelos
// O que tiver menor valor, significa que o texto pertence a essa classe


// Função para determinar a que classe o texto pertence
string determinateClass(){
}


int main(int argc,char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: "<< argv[0] << "<human_collection> <gpt_collection> <text_to_analyse>" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string textFile = readFile(argv[3]);

    return 0;
}