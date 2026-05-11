#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <sstream>
#include <thread>
#include <atomic>

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

//pega a memoria usada no momento que a função é chamada
size_t getCurrentMemoryKB()
{
    ifstream file("/proc/self/status");
    string line;
    while (getline(file, line))
    {
        if (line.find("VmRSS:") == 0)
        {
            stringstream ss(line);
            string key;
            size_t valueKB;
            string unit;
            ss >> key >> valueKB >> unit;
            return valueKB;
        }
    }
    return 0;
}

//Pega a media de memoria usada no programa
size_t getAvgMemoryUsageKB(atomic<bool>& running)
{
    size_t total = 0;
    size_t count = 0;

    while (running)
    {
        total += getCurrentMemoryKB();
        count++;
        this_thread::sleep_for(chrono::milliseconds(10)); // amostra a cada 10ms
    }

    return count > 0 ? (total / count) : 0;
}

//Pega Pico do uso de RAM
//Não verificado ainda, mantido como comentário por enquanto
size_t getPeakMemoryUsageKB()
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

            return valueKB;
        }
    }

    return 0; // não encontrado
}

//Pega Energia usada pelo processo
//Não verificado ainda, mantido como comentário
double readEnergyJoules()
{
    ifstream file("/sys/class/powercap/intel-rapl/intel-rapl:0/energy_uj");
    if (!file) return -1.0;

    long long microJoules;
    file >> microJoules;
    return microJoules / 1e6;
}

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
int medianOfThree(vector<int>& arr, int low, int high) {
    int mid = low + (high - low) / 2;

    int a = arr[low], b = arr[mid], c = arr[high];

    if ((a <= b && b <= c) || (c <= b && b <= a)) return b;
    if ((b <= a && a <= c) || (c <= a && a <= b)) return a;
    return c;
}

bool testCorrectness(vector<int>& original)
{
    size_t max = original.size();
    for(size_t i = 0; i < max - 1; i++)
    {
        if(original[i] > original[i+1]) return false;
    }
    return true;
}


//Quicksort main
void quickSort(vector<int>& arr, int low, int high) {
    if (low >= high) return;

    int pivot = medianOfThree(arr, low, high);

    int lt = low;
    int gt = high;
    int i  = low;

    while (i <= gt) {
        if (arr[i] < pivot) {
            swap(arr[lt], arr[i]);
            lt++; i++;
        } else if (arr[i] > pivot) {
            swap(arr[i], arr[gt]);
            gt--;
        } else {
            i++;
        }
    }

    quickSort(arr, low, lt - 1);
    quickSort(arr, gt + 1, high);
}

//Escrita do benchmark em um .CSV
//Estrutura Result para facilitar Escrita
//Ínicio código gerado por IA
struct Result
{
    string vectorType;
    string algorithm;
    size_t size;

    double timeMs;

    string stress;

    size_t ramMBmax;
    size_t ramMBmedia;

    double energyJ;
};

void writeCSV(ofstream& csv, const Result& r)
{
    csv
        << r.vectorType << ","
        << r.algorithm << ","
        << r.size<< ","
        << r.timeMs << ","
        << r.stress << ","
        << r.ramMBmax << ","
        << r.ramMBmedia << ","
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

    atomic<bool> monitoring(true);

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

    cout << algorithm << " " << datasetName << " " << original.size() << endl;

    //Energia, não testada, desativada
    //double energyBefore = readEnergyJoules();

    size_t avgRam = 0;

    thread monitorThread([&]() {
        avgRam = getAvgMemoryUsageKB(monitoring);
    });

    double energyBefore = readEnergyJoules();
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

    double energyAfter = readEnergyJoules();

    monitoring = false;
    monitorThread.join();
    
    

    if(!testCorrectness(original)) 
    {
        cout << "Algoritmo não funcionou corretamente" << endl;
        return -1;
    }
    //double energyAfter = readEnergyJoules();

    //Tempo total = tempo depois - tempo antes
    double timeMs = duration<double, milli>(end - start).count();

    //Arquivo .csv
    ofstream csv_alg("results/" + algorithm + ".csv", ios::app);

    //Header csv
    if (csv_alg.tellp() == 0)
    {
        csv_alg << "vector_type,algorithm,size,time_ms,stress,ram_kb_max,ram_kb_media,energy_j\n";
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
    r.size = original.size();
    r.stress = "None";
    r.ramMBmax = getPeakMemoryUsageKB(); 
    r.ramMBmedia = avgRam;
    r.energyJ = (energyAfter >= 0 && energyBefore >= 0) ? energyAfter - energyBefore : -1.0;
    //Escreve csv
    writeCSV(csv_alg, r);

    csv_alg.close();

    ofstream csv_data("results/" + datasetName + ".csv", ios::app);
    //Header csv
    if (csv_data.tellp() == 0)
    {
        csv_data << "vector_type,algorithm,size,time_ms,stress,ram_kb_max,ram_kb_media,energy_j\n";
    }

    writeCSV(csv_data, r);
    csv_data.close();

    ofstream csv_size("results/" + to_string(r.size) + ".csv", ios::app);
    //Header csv
    if (csv_size.tellp() == 0)
    {
        csv_size << "vector_type,algorithm,size,time_ms,stress,ram_kb_max,ram_kb_media,energy_j\n";
    }

    writeCSV(csv_size, r);
    csv_size.close();

    cout << "Benchmark finalizado." << endl;

    return 0;
}