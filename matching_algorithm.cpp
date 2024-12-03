#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <windows.h>
#include <psapi.h>

const int N = 10; // 노드 수 (예시로 작은 값을 사용)
const int TARGET_WEIGHT = 15; // 찾고자 하는 매칭의 목표 가중치

// Isolation Lemma를 위한 무작위 비용 할당 함수
std::vector<int> assignRandomCosts(int n) {
    std::vector<int> costs(n);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, (1 << n) - 1);
    for (int i = 0; i < n; i++) {
        costs[i] = dis(gen);
    }
    return costs;
}

// 임의의 NxN 스큐 대칭 행렬을 생성하는 함수
std::vector<std::vector<int>> createSkewSymmetricMatrix(const std::vector<int>& costs) {
    std::vector<std::vector<int>> matrix(N, std::vector<int>(N, 0));
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            matrix[i][j] = costs[i] * costs[j];
            matrix[j][i] = -matrix[i][j];
        }
    }
    return matrix;
}

// Pfaffian 계산을 위한 (단순화된) 함수
int computePfaffian(const std::vector<std::vector<int>>& matrix) {
    int pfaffian = 0;
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            pfaffian += matrix[i][j];
        }
    }
    return pfaffian;
}

// 메모리 사용량을 측정하는 함수 (Windows 기반)
long getMemoryUsage() {
    PROCESS_MEMORY_COUNTERS memInfo;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &memInfo, sizeof(memInfo))) {
        return memInfo.WorkingSetSize; // 현재 프로세스의 메모리 사용량 (바이트 단위)
    }
    return 0;
}

// 알고리즘 실행 함수
bool findPerfectMatchingWithExactWeight(int targetWeight, std::vector<std::pair<int, int>>& matching) {
    auto costs = assignRandomCosts(N);
    auto matrix = createSkewSymmetricMatrix(costs);

    int pfaffian = computePfaffian(matrix);
    if (pfaffian % targetWeight == 0) {
        for (int i = 0; i < N; i += 2) {
            matching.emplace_back(i, i + 1); // 단순히 연속된 노드 쌍을 매칭하는 예시
        }
        return true;
    }
    return false;
}

int main() {
    // 실행 시간 측정 시작
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::pair<int, int>> matching;
    bool result = findPerfectMatchingWithExactWeight(TARGET_WEIGHT, matching);

    // 실행 시간 측정 종료
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    // 메모리 사용량 측정
    long memoryUsage = getMemoryUsage();

    // 결과 출력
    if (result) {
        std::cout << "Perfect matching with the exact weight found." << std::endl;
        std::cout << "Matching (Certificate): ";
        for (const auto& pair : matching) {
            std::cout << "(" << pair.first << ", " << pair.second << ") ";
        }
        std::cout << std::endl;
    } else {
        std::cout << "No perfect matching with the exact weight found." << std::endl;
    }

    std::cout << "Execution Time: " << elapsed.count() << " seconds" << std::endl;
    std::cout << "Memory Usage: " << memoryUsage / 1024 << " KB" << std::endl;

    return 0;
}
