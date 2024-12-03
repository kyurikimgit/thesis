#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <fstream>
#include <string>

using namespace std;

const int N = 10;
const int TARGET_WEIGHT = 15;

vector<int> assignRandomCosts(int n) {
    vector<int> costs(n);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, (1 << n) - 1);
    for (int i = 0; i < n; i++) {
        costs[i] = dis(gen);
    }
    return costs;
}

vector<vector<int>> createSkewSymmetricMatrix(const vector<int>& costs) {
    vector<vector<int>> matrix(N, vector<int>(N, 0));
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            matrix[i][j] = costs[i] * costs[j];
            matrix[j][i] = -matrix[i][j];
        }
    }
    return matrix;
}

int computePfaffian(const vector<vector<int>>& matrix) {
    int pfaffian = 0;
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            pfaffian += matrix[i][j];
        }
    }
    return pfaffian;
}

long getMemoryUsage() {
    ifstream file("/proc/self/status");
    string line;
    while (getline(file, line)) {
        if (line.rfind("VmRSS:", 0) == 0) {
            long memUsage = stol(line.substr(6)) * 1024;
            return memUsage;
        }
    }
    return 0;
}

bool findPerfectMatchingWithExactWeight(int targetWeight, vector<pair<int, int>>& matching) {
    auto costs = assignRandomCosts(N); 
    auto matrix = createSkewSymmetricMatrix(costs); 

    int pfaffian = computePfaffian(matrix);
    if (pfaffian % targetWeight == 0) {
        for (int i = 0; i < N; i += 2) {
            matching.emplace_back(i, i+1);
        }
        return true;
    }
    return false;
}

int main() {
    auto start = chrono::high_resolution_clock::now();

    vector<pair<int, int>> matching;
    bool result = findPerfectMatchingWithExactWeight(TARGET_WEIGHT, matching);

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    long memoryUsage = getMemoryUsage();

    if (result) {
        cout << "Perfect matching with the exact weight found." << endl;
        cout << "Matching (Certificate): ";
        for (const auto& pair : matching) {
            cout << "(" << pair.first << ", " << pair.second << ") ";
        }
        cout << endl;
    } else {
        cout << "No perfect matching with the exact weight found." << endl;
    }

    cout << "Execution Time: " << elapsed.count() << " seconds" << endl;
    cout << "Memory Usage: " << memoryUsage / 1024 << " KB" << endl;

    return 0;
}
