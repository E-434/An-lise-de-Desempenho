#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <sstream>
#include <thread>
#include <atomic>
#include <cstdlib>
#include <ctime>
#include <cctype>
#include <stdexcept>
#include <filesystem>

using namespace std;
using namespace std::chrono;
namespace fs = std::filesystem;

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

//Usa Pico do uso de RAM
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

//Verifica Energia usada pelo processo
double readEnergyJoules()
{
    ifstream file("/sys/class/powercap/intel-rapl/intel-rapl:0/energy_uj");
    if (!file) return -1.0;

    long long microJoules;
    file >> microJoules;
    return microJoules / 1e6;
}


//Nível de estresse a ser aplicado durante o sorting
//Inicio do código criado por IA
enum class StressLevel {
    None,
    Cpu,
    Ram,
    Both
};

//Converte a entrada do usuário para o enum de estresse
StressLevel parseStressLevel(const string& value)
{
    string normalized;
    normalized.reserve(value.size());

    for (char c : value)
    {
        normalized.push_back(static_cast<char>(tolower(static_cast<unsigned char>(c))));
    }

    if (normalized == "none" || normalized == "nenhum" || normalized == "sem" || normalized == "semestresse")
        return StressLevel::None;

    if (normalized == "cpu" || normalized == "processador")
        return StressLevel::Cpu;

    if (normalized == "ram" || normalized == "memoria" || normalized == "memória")
        return StressLevel::Ram;

    if (normalized == "both" || normalized == "ambos" || normalized == "cpu+ram")
        return StressLevel::Both;

    throw invalid_argument("Nível de estresse inválido. Use: none, cpu, ram ou both.");
}

//Converte o enum de estresse para string para salvar no CSV
string stressLevelToString(StressLevel level)
{
    switch (level)
    {
        case StressLevel::None: return "none";
        case StressLevel::Cpu: return "cpu";
        case StressLevel::Ram: return "ram";
        case StressLevel::Both: return "both";
    }

    return "none";
}

//Aplica estresse em CPU enquanto o sorting estiver sendo executado
void runCpuStress(std::atomic<bool>& running)
{
    volatile double x = 1.1;

    while (running)
    {
        //Executa operações aritméticas simples para consumir CPU durante o sorting
        x *= 1.0000001;
        x /= 1.00000009;
        x += 0.1234;
        x -= 0.5678;
        x *= 1.0000003;
        x /= 1.0000002;
        x += x * 0.000001;
    }
}

//Aplica estresse em RAM enquanto o sorting estiver sendo executado
void runRamStress(std::atomic<bool>& running)
{
    constexpr size_t chunkSize = 512ULL * 1024ULL * 1024ULL;
    constexpr size_t mask = chunkSize - 1;

    std::vector<unsigned char> buffer(chunkSize, 0);

    uint64_t state = 0x123456789ABCDEFULL;
    unsigned char value = 0;

    while (running)
    {
        //Realiza leituras e escritas em um buffer grande para aumentar o uso de RAM
        for (int i = 0; i < 8; i++)
        {
            state ^= state << 13;
            state ^= state >> 7;
            state ^= state << 17;

            size_t index = state & mask;

            value ^= buffer[index];
            buffer[index] = value;
        }
    }
}

//Inicia as threads de estresse conforme o nível escolhido
void startStress(StressLevel level, atomic<bool>& running, vector<thread>& workers)
{
    if (level == StressLevel::None)
        return;

    if (level == StressLevel::Cpu || level == StressLevel::Both)
    {
        workers.emplace_back([&]() {
            runCpuStress(running);
        });
    }

    if (level == StressLevel::Ram || level == StressLevel::Both)
    {
        workers.emplace_back([&]() {
            runRamStress(running);
        });
    }
}

//Para as threads de estresse ao final da execução do sorting
void stopStress(vector<thread>& workers, atomic<bool>& running)
{
    running = false;

    for (thread& worker : workers)
    {
        if (worker.joinable())
            worker.join();
    }

    workers.clear();
}
//Fim do código criado por IA
//Estrutura para armazenar estatísticas do sorting
struct SortingStats
{
    size_t comparisons = 0;
    size_t swaps = 0;
};

SortingStats g_stats;

void resetStats()
{
    g_stats.comparisons = 0;
    g_stats.swaps = 0;
}

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
    if (l < n)
    {
        g_stats.comparisons++;
        if (arr[l] > arr[largest])
            largest = l;
    }

    //Se filho da direita, for maior que o maior número
    if (r < n)
    {
        g_stats.comparisons++;
        if (arr[r] > arr[largest])
            largest = r;
    }

    // Se maior não é a raiz
    if (largest != i) {
        g_stats.swaps++;
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
        g_stats.swaps++;
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
        g_stats.comparisons++;
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


bool testCorrectness(vector<int>& original)
{
    size_t max = original.size();
    for(size_t i = 0; i < max - 1; i++)
    {
        if(original[i] > original[i+1]) return false;
    }
    return true;
}

int partition(vector<int>& arr, int low, int high) {
    int mid = low + (high - low) / 2;  // evita overflow vs (low+high)/2
    g_stats.swaps++;
    swap(arr[mid], arr[high]);          // move o pivô pro final

    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        g_stats.comparisons++;
        if (arr[j] == pivot) {
            if (rand() % 2 != 0) {
                i++;
                g_stats.swaps++;
                swap(arr[i], arr[j]);
            }
        }
        else {
            g_stats.comparisons++;
            if (arr[j] < pivot) {
                i++;
                g_stats.swaps++;
                swap(arr[i], arr[j]);
            }
        }
    }

    g_stats.swaps++;
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

void quickSort(vector<int>& arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
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
    size_t size;

    double timeMs;

    string stress;

    size_t ramMBmax;
    size_t ramMBmedia;

    double energyJ;
    size_t comparisons;
    size_t swaps;
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
        << r.energyJ << ","
        << r.comparisons << ","
        << r.swaps
        << "\n";
}


//main
int main(int argc, char* argv[])
{
    //Verifica entrada correta
    if (argc != 3 && argc != 4)
    {
        cerr << "Uso: ./sorts <algoritmo> <arquivo_bin> [nivel_estresse]" << endl;
        cerr << "Algoritmos válidos: heap, merge, quick" << endl;
        cerr << "Níveis de estresse válidos: none, cpu, ram, both" << endl;

        return 1;
    }

    atomic<bool> monitoring(true);

    //Salva entradas
    string algorithm = argv[1];
    string datasetPath = argv[2];
    StressLevel stressLevel = StressLevel::None;

    if (argc == 4)
    {
        try
        {
            stressLevel = parseStressLevel(argv[3]);
        }
        catch (const exception& e)
        {
            cerr << e.what() << endl;
            return 1;
        }
    }

    fs::path datasetFsPath(datasetPath);
    string datasetName = datasetFsPath.stem().string();
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

    resetStats();

    cout << algorithm << " " << datasetName << " " << original.size() << endl;
    cout << "Estresse: " << stressLevelToString(stressLevel) << endl;

    size_t avgRam = 0;
    vector<thread> stressThreads;

    startStress(stressLevel, monitoring, stressThreads);

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
    stopStress(stressThreads, monitoring);
    monitorThread.join();

    if(!testCorrectness(original)) 
    {
        cout << "Algoritmo não funcionou corretamente" << endl;
        return -1;
    }

    //Tempo total = tempo depois - tempo antes
    double timeMs = duration<double, milli>(end - start).count();

    //Arquivo .csv
    fs::path csvAlgPath = fs::path("results") / (algorithm + ".csv");
    bool writeHeaderAlg = !fs::exists(csvAlgPath) || fs::file_size(csvAlgPath) == 0;
    ofstream csv_alg(csvAlgPath, ios::app);

    if (writeHeaderAlg)
    {
        csv_alg << "vector_type,algorithm,size,time_ms,stress,ram_kb_max,ram_kb_media,energy_j,comparisons,swaps\n";
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
    //Salva resultados
    r.timeMs = timeMs;
    r.size = original.size();
    r.stress = stressLevelToString(stressLevel);
    r.comparisons = g_stats.comparisons;
    r.swaps = g_stats.swaps;
    //Salva RAM usada
    r.ramMBmax = getPeakMemoryUsageKB(); 
    r.ramMBmedia = avgRam;
    //Cálculo de energia usada, caso não seja possível pegar energia, retorna -1.0
    r.energyJ = (energyAfter >= 0 && energyBefore >= 0) ? energyAfter - energyBefore : -1.0;
    //Escreve csv
    writeCSV(csv_alg, r);

    csv_alg.close();

    fs::path csvDataPath = fs::path("results") / (datasetName + ".csv");
    bool writeHeaderData = !fs::exists(csvDataPath) || fs::file_size(csvDataPath) == 0;
    ofstream csv_data(csvDataPath, ios::app);
    if (writeHeaderData)
    {
        csv_data << "vector_type,algorithm,size,time_ms,stress,ram_kb_max,ram_kb_media,energy_j,comparisons,swaps\n";
    }

    writeCSV(csv_data, r);
    csv_data.close();

    fs::path csvSizePath = fs::path("results") / (to_string(r.size) + ".csv");
    bool writeHeaderSize = !fs::exists(csvSizePath) || fs::file_size(csvSizePath) == 0;
    ofstream csv_size(csvSizePath, ios::app);
    if (writeHeaderSize)
    {
        csv_size << "vector_type,algorithm,size,time_ms,stress,ram_kb_max,ram_kb_media,energy_j,comparisons,swaps\n";
    }

    writeCSV(csv_size, r);
    csv_size.close();

    cout << "Benchmark finalizado." << endl;

    return 0;
}