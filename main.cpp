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

void run_size_tests(const int rows, const int cols, const int k, std::fstream &file) {
    const std::vector<std::vector<int>> matrixA_ini = generate_matrix(rows, cols);
    const std::vector<std::vector<int>> matrixB_ini = generate_matrix(rows, cols);
    std::vector<std::vector<int>> matrixA = matrixA_ini;
    std::vector<std::vector<int>> matrixB = matrixB_ini;

    for (const std::vector threads = {1, 2, 4, 8, 10, 20, 40, 80, 160}; const int thread : threads) {
        auto start = high_resolution_clock::now();
        multi_threads(matrixA, matrixB, k, thread);
        auto end = high_resolution_clock::now();
        const auto duration = static_cast<double>(duration_cast<nanoseconds>(end - start).count())*1e-9;
        const bool correct = check(k, matrixA_ini, matrixB, matrixA);

        file << thread << ',' << correct << ',' << duration
        << ',' << matrixA.size() << ',' << matrixA[0].size() << '\n';

        matrixA = matrixA_ini;
        matrixB = matrixB_ini;
    }
}

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-1000, 1000);

    const int k = static_cast<int>(dis(gen));

    std::fstream csvfile("test_results.csv");
    if (!csvfile.is_open()) {
        std::cout << "Unable to open file" << std::endl;
    }
    csvfile << "threads,correct,duration,rows,cols\n";

    run_size_tests(10, 10, k, csvfile);
    run_size_tests(100, 100, k, csvfile);
    run_size_tests(500, 500, k, csvfile);
    run_size_tests(1000, 1000, k, csvfile);

    return 0;
}