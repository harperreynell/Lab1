#include <iostream>
#include <thread>
#include <random>
#include <fstream>

using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::nanoseconds;

std::vector<std::vector<int>> generate_matrix(const int rows, const int cols) {
    std::vector matrix(rows, std::vector<int>(cols));

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-100, 100);
    for (auto & row : matrix) {
        for (auto &element : row) {
            element = static_cast<int>(dis(gen));
        }
    }

    return matrix;
}

void single_thread(std::vector<std::vector<int>> &matrixA, std::vector<std::vector<int>> matrixB, const int k) {
    for (int i = 0; i < matrixA.size(); i++) {
        for (int j = 0; j < matrixA[0].size(); j++) {
            matrixA[i][j] -= matrixB[i][j]*k;
        }
    }
}

void threading_chunk(std::vector<std::vector<int>> &matrixA, const std::vector<std::vector<int>> &matrixB, const int k,
    const int start, const int end) {
    for (int i = start; i < end; i++) {
        for (int j = 0; j < matrixA[i].size(); j++) {
            matrixA[i][j] -= matrixB[i][j] * k;
        }
    }
}

void print_matrix(const std::vector<std::vector<int>> &matrix) {
    for (auto & row : matrix) {
        for (auto & element : row) {
            std::cout << element << " ";
        }
        std::cout << std::endl;
    }
}

bool check(const int k, const std::vector<std::vector<int>> &matrixA, const std::vector<std::vector<int>> &matrixB, const std::vector<std::vector<int>> &result_matrix) {
    if (matrixA.size() != result_matrix.size()) {
        return false;
    }
    if (matrixA[0].size() != result_matrix[0].size()) {
        return false;
    }
    for (int i = 0; i < matrixA.size(); i++) {
        for (int j = 0; j < matrixA[0].size(); j++) {
            if (matrixA[i][j] != result_matrix[i][j] + matrixB[i][j]*k) {
                return false;
            }
        }
    }

    return true;
}

void multi_threads(std::vector<std::vector<int>> &matrixA, std::vector<std::vector<int>> &matrixB, const int k, const int threads_cnt=4) {
    const int rows = static_cast<int>(matrixA.size());
    std::vector<std::thread> threads;
    int rows_thread = rows / threads_cnt;
    int current = 0;

    for (int i = 0; i < threads_cnt; i++) {
        int end = current + rows_thread;
        if (i == threads_cnt - 1) {
            end = rows;
        }
        threads.emplace_back(threading_chunk, std::ref(matrixA), std::ref(matrixB), k, current, end);
        current = end;
    }

    for (auto & thread : threads) {
        thread.join();
    }
}

int main() {
    constexpr int rows = 1000, cols = 1000;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-1000, 1000);

    const int k = static_cast<int>(dis(gen));

    const std::vector<std::vector<int>> matrixA_ini = generate_matrix(rows, cols);
    const std::vector<std::vector<int>> matrixB_ini = generate_matrix(rows, cols);
    std::vector<std::vector<int>> matrixA = matrixA_ini;
    std::vector<std::vector<int>> matrixB = matrixB_ini;

    auto start = high_resolution_clock::now();
    single_thread(matrixA, matrixB, k);
    auto end = high_resolution_clock::now();
    const auto duration = static_cast<double>(duration_cast<nanoseconds>(end - start).count())*1e-9;

    std::cout << "\nSingle thread calculation:\n";
    if (check(k, matrixA_ini, matrixB, matrixA)) {
        std::cout << "Matrix is correctly calculated in " << duration << " seconds\n";
    } else {
        std::cout << "Matrix is NOT calculated correctly in " << duration << " seconds\n";
    }

    matrixA = matrixA_ini;
    matrixB = matrixB_ini;

    std::cout << "\n\nMulti thread calculation: \n";

    start = high_resolution_clock::now();
    multi_threads(matrixA, matrixB, k);
    end = high_resolution_clock::now();
    const auto multi_duration = static_cast<double>(duration_cast<nanoseconds>(end - start).count())*1e-9;

    if (check(k, matrixA_ini, matrixB, matrixA)) {
        std::cout << "Matrix is correctly calculated in " << multi_duration << " seconds\n";
    } else {
        std::cout << "Matrix is NOT calculated correctly in " << multi_duration << " seconds\n";
    }

    return 0;
}