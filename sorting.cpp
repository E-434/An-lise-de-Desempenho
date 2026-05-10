#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <sstream>

using namespace std;
using namespace std::chrono;

//Função que carrega os vetores salvos na pasta datasets como .bins
//Início do código criado por IA
vector<int> loadVector(const string& filename)
{
    ifstream file(filename, ios::binary);

    if (!file)
    {
        cerr << "Erro ao abrir arquivo: " << filename << endl;
        exit(1);
    }

    size_t size;

    file.read(reinterpret_cast<char*>(&size), sizeof(size_t));

    vector<int> vec(size);

    file.read(reinterpret_cast<char*>(vec.data()), size * sizeof(int));

    file.close();

    return vec;
}

//Pega Pico do uso de RAM
//Não verificado ainda, mantido como comentário por enquanto
/*size_t getPeakMemoryUsageMB()
{
    ifstream file("/proc/self/status");

    string line;

    while (getline(file, line))
    {
        if (line.find("VmHWM:") == 0)
        {
            stringstream ss(line);

            string key;
            size_t valueKB;
            string unit;

            ss >> key >> valueKB >> unit;

            return valueKB / 1024;
        }
    }

}*/

//Pega Energia usada pelo processo
//Não verificado ainda, mantido como comentário
/*double readEnergyJoules()
{
    //Ajustar de acordo com o sistema Linux usado
    ifstream file("/sys/class/powercap/intel-rapl:0/energy_uj");

    if (!file)
    {
        return -1.0;
    }

    long long microJoules;

    file >> microJoules;

    return microJoules / 1e6;
}*/

//Fim do código gerado por IA

//Algoritmo de heapsort
//Heapify sub-árvore com raiz i
void heapify(vector<int>& arr, int n, int i){

    //Raiz é o maior número
    int largest = i;

    //Index esquerdo
    int l = 2 * i + 1;

    //Index Direito
    int r = 2 * i + 2;

    //Se filho da esquerda, for maior que o maior número
    if (l < n && arr[l] > arr[largest])
        largest = l;

    //Se filho da direita, for maior que o maior número
    if (r < n && arr[r] > arr[largest])
        largest = r;

    // Se maior não é a raiz
    if (largest != i) {
        swap(arr[i], arr[largest]);

        //Reorganiza recursivamente
        heapify(arr, n, largest);
    }
}

//HeapSort main
void heapSort(vector<int>& arr){
    int n = arr.size();

    //Controi o heap
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    //Extraia cada elemento do heap
    for (int i = n - 1; i > 0; i--) {

        //Mova raiz atual para o fim
        swap(arr[0], arr[i]);

        //Chame heapify no restante
        heapify(arr, i, 0);
    }
}

//Mergesort
//Faz o merge dos dois subarrays
void merge(vector<int>& arr, int left, int mid, int right){
                         
    int n1 = mid - left + 1;
    int n2 = right - mid;

    //Vetores temporais
    vector<int> L(n1), R(n2);

    //Copia dados pros vetores temporários
    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    int i = 0, j = 0;
    int k = left;

    //Faz o merge dos vetores temporários
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    // Copie os elementos que restam de L[], se existem
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    // Copie os elementos que restam de R[], se existem
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

// Mergesort main
void mergeSort(vector<int>& arr, int left, int right){
    
    if (left >= right)
        return;

    int mid = left + (right - left) / 2;
    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

//Quicksort
//Pivô mediana
int medianOfThree(vector<int>& arr, int low, int high)
{
    int mid = low + (high - low) / 2;

    if (arr[low] > arr[mid])
        swap(arr[low], arr[mid]);

    if (arr[low] > arr[high])
        swap(arr[low], arr[high]);

    if (arr[mid] > arr[high])
        swap(arr[mid], arr[high]);

    swap(arr[mid], arr[high]);

    return arr[high];
}

//Partição do quicksort
int partition(vector<int>& arr, int low, int high) {
  
    //Pivô
    int pivot = medianOfThree(arr, low, high);
  
    //Última posição fora do range
    int i = low - 1;

    //Move todos elementos menores que o pivô para sua esquerda
    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    
    //PivÔ vai para a posição logo depois de elementos menores que ele e retorna sua posição
    swap(arr[i + 1], arr[high]);  
    return i + 1;
}

//Quicksort main
void quickSort(vector<int>& arr, int low, int high) {
  
    if (low < high) {
      
        //Index da última partição
        int pi = partition(arr, low, high);

        //Chamada recursiva para os dois lados do pivÔ
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

//Escrita do benchmark em um .CSV
//Estrutura Result para facilitar Escrita
//Ínicio código gerado por IA
struct Result
{
    string vectorType;
    string algorithm;

    double timeMs;

    string stress;

    size_t ramMB;

    double energyJ;
};

void writeCSV(ofstream& csv, const Result& r)
{
    csv
        << r.vectorType << ","
        << r.algorithm << ","
        << r.timeMs << ","
        << r.stress << ","
        << r.ramMB << ","
        << r.energyJ
        << "\n";
}


//main
int main(int argc, char* argv[])
{
    //Verifica entrada correta
    if (argc != 3)
    {
        cerr << "Uso: ./sorts <algoritmo> <arquivo_bin>" << endl;
        cerr << "Algoritmos válidos: heap, merge, quick" << endl;

        return 1;
    }
    //Salva entradas
    string algorithm = argv[1];
    string datasetPath = argv[2];

    string datasetName = datasetPath;
    //Acha .bin com vetor
    size_t slashPos = datasetName.find_last_of("/");

    if (slashPos != string::npos)
        datasetName = datasetName.substr(slashPos + 1);

    size_t dotPos = datasetName.find(".");

    if (dotPos != string::npos)
        datasetName = datasetName.substr(0, dotPos);
    //Verifica algoritmo
    if (
        algorithm != "heap" &&
        algorithm != "merge" &&
        algorithm != "quick"
    )
    {
        cerr << "Algoritmo inválido." << endl;
        return 1;
    }

    vector<int> original = loadVector(datasetPath);

    //Energia, não testada, desativada
    //double energyBefore = readEnergyJoules();

    //Começa tempo
    auto start = high_resolution_clock::now();

    //Roda algoritmo
    if (algorithm == "heap")
    {
        heapSort(original);
    }
    else if (algorithm == "merge")
    {
        mergeSort(original, 0, original.size() - 1);
    }
    else if (algorithm == "quick")
    {
        quickSort(original, 0, original.size() - 1);
    }
    //Termina tempo
    auto end = high_resolution_clock::now();

    //double energyAfter = readEnergyJoules();

    //Tempo total = tempo depois - tempo antes
    double timeMs = duration<double, milli>(end - start).count();

    //Arquivo .csv
    ofstream csv("results/results.csv", ios::app);

    //Header csv
    if (csv.tellp() == 0)
    {
        csv << "vector_type,algorithm,time_ms,stress,ram_mb,energy_j\n";
    }

    //Struct de resultado
    Result r;

    r.vectorType = datasetName;

    if (algorithm == "heap")
        r.algorithm = "HeapSort";
    else if (algorithm == "merge")
        r.algorithm = "MergeSort";
    else
        r.algorithm = "QuickSort";

    r.timeMs = timeMs;
    r.stress = "None";
    r.ramMB = 0;
    r.energyJ = 0.0;
    //Escreve csv
    writeCSV(csv, r);

    csv.close();

    cout << "Benchmark finalizado." << endl;

    return 0;
}