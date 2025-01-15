#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/max_cardinality_matching.hpp>
#include <random>
#include <chrono>
#include <cassert>

// Boost Graph Library configuration
using namespace boost;
typedef adjacency_list<vecS, vecS, undirectedS, no_property, property<edge_weight_t, int>> Graph;

// Pfaffian calculation function
int calculatePfaffian(std::vector<std::vector<int>>& matrix) {
    // If the matrix size is odd, add dummy data
    if (matrix.size() % 2 != 0) {
        matrix.emplace_back(std::vector<int>(matrix.size(), 0)); // Add a dummy row
        for (auto& row : matrix) {
            row.push_back(0);  // Add a dummy column
        }
    }

    assert(matrix.size() % 2 == 0);  // Verify that the size is even
    int n = matrix.size();
    int pfaffian = 1;
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (matrix[i][j] != 0) {
                pfaffian *= matrix[i][j];
                matrix[j][i] = matrix[i][j] = 0;
            }
        }
    }
    return pfaffian;
}

// MVV algorithm (Finding a matching)
bool findMatching(const std::vector<std::vector<int>>& graph, int targetWeight) {
    int n = graph.size();
    std::vector<bool> visited(n, false);
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (graph[i][j] == targetWeight && !visited[i] && !visited[j]) {
                visited[i] = visited[j] = true;
                break;
            }
        }
    }
    return std::all_of(visited.begin(), visited.end(), [](bool v) { return v; });
}

// Monte Carlo matching algorithm
bool monteCarloMatchingAlgorithm(const std::vector<std::vector<int>>& graph, int targetWeight, int numIterations) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 2 * graph.size());

    bool result = false;
    for (int i = 0; i < numIterations; ++i) {
        int randomCost = dis(gen);
        if (findMatching(graph, targetWeight)) {
            result = true;
            break;
        }
    }
    return result;
}

// Dataset reading function
void readDataset(const std::string& filepath, int& binWeightCap, int& binVolumeCap, std::vector<std::tuple<int, int, int>>& items) {
    std::ifstream file(filepath);
    if (!file) {
        throw std::runtime_error("Could not open file: " + filepath);
    }

    std::string line;
    std::getline(file, line);
    std::istringstream iss(line);
    iss >> binWeightCap >> binVolumeCap;

    while (std::getline(file, line)) {
        int index, demand, weight, volume;
        std::istringstream itemStream(line);
        itemStream >> index >> demand >> weight >> volume;
        items.emplace_back(demand, weight, volume);
    }
}

// Solve the graph matching problem
bool solveMatchingProblem(const std::vector<std::tuple<int, int, int>>& items, int binWeightCap, int binVolumeCap, int targetWeight) {
    int n = items.size();
    Graph g(n);

    // Add edges to the graph
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            int weight1 = std::get<1>(items[i]);
            int weight2 = std::get<1>(items[j]);
            if (weight1 + weight2 <= binWeightCap) {
                add_edge(i, j, g);
            }
        }
    }

    // Generate a matrix for Pfaffian calculation
    std::vector<std::vector<int>> matrix(n, std::vector<int>(n, 0));
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            matrix[i][j] = matrix[j][i] = 1;  // Fill with simple connection information
        }
    }

    int pfaffian = calculatePfaffian(matrix);
    std::cout << "Pfaffian: " << pfaffian << std::endl;

    // Perform Monte Carlo matching algorithm
    bool matchingExists = monteCarloMatchingAlgorithm(matrix, targetWeight, 100);
    return matchingExists;
}

// Runtime measurement function
double measureRuntime(std::function<void()> func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    return elapsed.count();
}

int main() {
    // Set the dataset path
    std::string datasetDir = "C:\\Users\\rlarb\\thesis_project\\2-dimensional\\";
    std::string filename = "CL_01_25_01.txt";
    std::string filepath = datasetDir + filename;

    int binWeightCap, binVolumeCap;
    std::vector<std::tuple<int, int, int>> items;

    // Read the dataset
    try {
        readDataset(filepath, binWeightCap, binVolumeCap, items);
    } catch (const std::exception& e) {
        std::cerr << "Error reading dataset: " << e.what() << std::endl;
        return 1;
    }

    int targetWeight = 10;

    // Solve the matching problem
    double runtime = measureRuntime([&]() {
        bool result = solveMatchingProblem(items, binWeightCap, binVolumeCap, targetWeight);
        if (result) {
            std::cout << "Perfect matching found!" << std::endl;
        } else {
            std::cout << "No perfect matching found." << std::endl;
        }
    });

    std::cout << "Runtime: " << runtime << " seconds" << std::endl;
    return 0;
}
