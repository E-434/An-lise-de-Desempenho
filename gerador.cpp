#include <iostream>
#include <random>
#include <vector>
#include <fstream>
#include <algorithm>
#include <climits>
#include <cmath>

using namespace std;

size_t SCALE = 4;
size_t VECTOR_SIZE = 0;

mt19937 rng;

//Código Gerado por IA - Início
//Salva vetor num .bin com o nome passado
void saveVector(const vector<int>& vec, const string& filename)
{
    ofstream file(filename, ios::binary);

    if (!file)
    {
        cerr << "Erro ao criar arquivo: " << filename << endl;
        exit(1);
    }

    size_t size = vec.size();

    file.write(reinterpret_cast<const char*>(&size), sizeof(size_t));

    file.write(reinterpret_cast<const char*>(vec.data()), size * sizeof(int));

    file.close();
}
//Código Gerado por IA - Fim

//Gera vetor ordenado
vector<int> GenerateOrdered(){
    vector<int> vec(VECTOR_SIZE);
    for(size_t i=0; i<VECTOR_SIZE; i++){
        vec[i] = i;
    }
    return vec;
}

//Gera vetor reverso
vector<int> GenerateReversed(){
    vector<int> vec(VECTOR_SIZE);
    for(size_t i=0; i<VECTOR_SIZE; i++){
        vec[i] = VECTOR_SIZE-i;
    }
    return vec;
}

//Gera vetor random
vector<int> GenerateRandom(){
    vector<int> vec(VECTOR_SIZE);
    uniform_int_distribution<int> dist(0, INT_MAX);
    for(size_t i=0; i<VECTOR_SIZE; i++){
        vec[i] = dist(rng);
    }
    return vec;
}

//Gera vetor repetido
vector<int> GenerateRepeated(){
    vector<int> vec(VECTOR_SIZE, 10);
    return vec;
}

//Gera vetor repetido com range 1 a 5
vector<int> GenerateRepeated1to5(){
    vector<int> vec(VECTOR_SIZE);
    uniform_int_distribution<int> dist(1, 5);
    for(size_t i=0; i<VECTOR_SIZE; i++){
        vec[i] = dist(rng);
    }
    return vec;
}

//Gera vetor zig-zag
vector<int> GenerateZigZag(){
    vector<int> vec(VECTOR_SIZE);
    int low = 0;
    int high = INT_MAX;
    for(size_t i=0; i<VECTOR_SIZE; i++){
        if(i%2==0){
            vec[i] = low;
            low++;
        }
        else{
            vec[i] = high;
            high--;
        }
    }
    return vec;
}

//Gera vetor como ruído no fim
vector<int> GenerateEndNoise(){
    vector<int> vec(VECTOR_SIZE);
    size_t sizeNoise = VECTOR_SIZE * 0.01;
    uniform_int_distribution<int> dist(0, INT_MAX);
    for(size_t i=0; i<VECTOR_SIZE; i++){
        if(i<VECTOR_SIZE-sizeNoise){
            vec[i] = i;
        }
        else{
            vec[i] = dist(rng);
        }
    }
    return vec;
}

//Gera vetor com swaps aleatórios
vector<int> GenerateRandomSwaps(){
    vector<int> vec(VECTOR_SIZE);
    for (size_t i = 0; i < VECTOR_SIZE; i++)
        vec[i] = i;
    size_t swaps = VECTOR_SIZE * 0.01;
    uniform_int_distribution<int> dist(0, VECTOR_SIZE-1);
    for (size_t i = 0; i < swaps; i++)
    {
        size_t a = dist(rng);
        size_t b = dist(rng);

        swap(vec[a], vec[b]);
    }
    return vec;
}

//Progama recebe seed como argumento de entrada, caso não receba usa default 1
int main(int argc, char* argv[]){
    //Código Gerado por IA - Início
    
    if (argc > 1) {
        try {
            SCALE = static_cast<size_t>(stoul(argv[1]));
        } catch (const exception& e) {
            cerr << "Entrada inválida: " << e.what() << endl;
            return 1;
        }
    }
    VECTOR_SIZE = static_cast<size_t>(pow(10, SCALE));
    unsigned int seed = 1;
    if (argc > 2)
        seed = static_cast<unsigned int>(stoul(argv[2]));
    //Código Gerado por IA - Fim
    //Salva Seed
    rng.seed(seed);
    cout << "Seed used:" << seed << endl;
    cout << "Ordered Vector" << endl;
    saveVector(GenerateOrdered(), "datasets/ordered.bin");
    cout << "Reversed Vector" << endl;
    saveVector(GenerateReversed(), "datasets/reversed.bin");
    cout << "Random Vector" << endl;
    saveVector(GenerateRandom(), "datasets/random.bin");
    cout << "Repeated Vector" << endl;
    saveVector(GenerateRepeated(), "datasets/repeated.bin");
    cout << "Repeated 1-5 Vector" << endl;
    saveVector(GenerateRepeated1to5(), "datasets/repeated_1_5.bin");
    cout << "ZigZag Vector" << endl;
    saveVector(GenerateZigZag(), "datasets/zigzag.bin");
    cout << "End Noise Vector" << endl;
    saveVector(GenerateEndNoise(), "datasets/endnoise.bin");
    cout << "Random Swaps Vector" << endl;
    saveVector(GenerateRandomSwaps(), "datasets/random_swaps.bin");
    cout << "Datasets Generated" << endl;
}