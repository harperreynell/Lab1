#include <iostream>
#include <thread>
#include <random>

using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::nanoseconds;

std::vector<std::vector<int>> generate_matrix(int rows, int cols) {
    std::vector matrix(rows, std::vector<int>(cols));

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-100, 100);
    for (auto & row : matrix) {
        for (auto &element : row) {
            element = dis(gen);
        }
    }

    return matrix;
}

void print_matrix(std::vector<std::vector<int>> matrix) {
    for (auto & row : matrix) {
        for (auto & element : row) {
            std::cout << element << " ";
        }
        std::cout << std::endl;
    }
}

std::vector<std::vector<int>> multiply_matrix(const std::vector<std::vector<int>> &matrix, const int rows, const int cols, const int k) {
    std::vector result(rows, std::vector<int>(cols));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result[i][j] = matrix[i][j] * k;
        }
    }

    return result;
}

bool check(std::vector<std::vector<int>> &matrix, std::vector<std::vector<int>> &result_matrix, const int k) {
    if (matrix.size() != result_matrix.size()) {
        return false;
    }
    if (matrix[0].size() != result_matrix[0].size()) {
        return false;
    }

    for (int i = 0; i < matrix.size(); i++) {
        for (int j = 0; j < result_matrix[0].size(); j++) {
            if (result_matrix[i][j] != matrix[i][j]*k) {
                return false;
            }
        }
    }

    return true;
}

std::vector<std::vector<int>> substract_matrix(const::std::vector<std::vector<int>> &matrixA,const::std::vector<std::vector<int>> &matrixB, const int rows, const int cols) {
    std::vector<std::vector<int>> result_matrix(rows, std::vector<int>(cols));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result_matrix[i][j] = matrixA[i][j] - matrixB[i][j];
        }
    }
    return result_matrix;
}

bool final_check(std::vector<std::vector<int>> &matrixA, std::vector<std::vector<int>> &matrixB, std::vector<std::vector<int>> &result_matrix) {
    if (matrixA.size() != result_matrix.size()) {
        return false;
    }
    if (matrixA[0].size() != result_matrix[0].size()) {
        return false;
    }
    for (int i = 0; i < matrixA.size(); i++) {
        for (int j = 0; j < matrixA[0].size(); j++) {
            if (result_matrix[i][j] != matrixA[i][j] - matrixB[i][j]) {
                return false;
            }
        }
    }

    return true;
}

void matrix_mult_chunk(const int start, const int end, const int k, std::vector<std::vector<int>> &matrix) {
    for (int i = start; i < end; i++) {
        for (int j = 0; j < matrix[i].size(); j++) {
            matrix[i][j] = matrix[i][j] * k;
        }
    }
}

void matrix_sub_chunk(const int start, const int end, std::vector<std::vector<int>> &matrixA, const std::vector<std::vector<int>> &matrixB) {
    for (int i = start; i < end; i++) {
        for (int j = 0; j < matrixA[i].size(); j++) {
            matrixA[i][j] = matrixA[i][j] - matrixB[i][j];
        }
    }
}

void multi_matrix_threads(std::vector<std::vector<int>> &matrix, const int rows, const int k) {
    int threads_cnt = 4;

    std::vector<std::thread> threads;
    int rows_thread = rows / threads_cnt;
    int current = 0;

    for (int i = 0; i < threads_cnt; i++) {
        int end = current + rows_thread;
        if (i == threads_cnt - 1) {
            end = rows;
        }
        threads.emplace_back(matrix_mult_chunk, current, end, k, std::ref(matrix));
        current = end;
    }

    for (auto & thread : threads) {
        thread.join();
    }
}

void sub_matrix_threads(std::vector<std::vector<int>> &matrixA, std::vector<std::vector<int>> &matrixB, const int rows) {
    int threads_cnt = 4;

    std::vector<std::thread> threads;
    int rows_thread = rows / threads_cnt;
    int current = 0;

    for (int i = 0; i < threads_cnt; i++) {
        int end = current + rows_thread;
        if (i == threads_cnt - 1) {
            end = rows;
        }
        threads.emplace_back(matrix_sub_chunk, current, end, std::ref(matrixA), std::ref(matrixB));
        current = end;
    }

    for (auto & thread : threads) {
        thread.join();
    }
}

int main() {
    const int rows = 1000, cols = 1000;
    std::vector<std::vector<int>> matrixA = generate_matrix(rows, cols);
    std::vector<std::vector<int>> matrixB = generate_matrix(rows, cols);

    std::vector<std::vector<int>> matrixA_ini = matrixA;
    std::vector<std::vector<int>> matrixB_ini = matrixB;

    std::cout << "Initial matrix A:\n";
    // print_matrix(matrixA);
    std::cout << "Initial matrix B:\n";
    // print_matrix(matrixB);

    const int k = 10;

    auto start = high_resolution_clock::now();
    std::vector<std::vector<int>> result = multiply_matrix(matrixB, rows, cols, k);
    auto end = high_resolution_clock::now();
    auto multiplication_duration = duration_cast<nanoseconds>(end - start).count()*1e-9;

    start = high_resolution_clock::now();
    std::vector<std::vector<int>> matrixC = substract_matrix(matrixA, result, rows, cols);
    end = high_resolution_clock::now();
    auto subtraction_duration = duration_cast<nanoseconds>(end - start).count()*1e-9;

    bool check1 = check(matrixB, result, k);
    bool check2 = final_check(matrixA, matrixC, result);
    std::cout << "\nMatrix C=A-k*B = \n";
    print_matrix(matrixC);

    std::cout << "\nMatrix check:\n";
    if (check1) {
        std::cout << "Matrix multiplication by " << k << " is correct \n\tTime elapsed: " <<
            multiplication_duration << " seconds\n";
    } else {
        std::cout << "Matrix multiplication by " << k << " is not correct \n\tTime elapsed: " <<
            multiplication_duration << " seconds\n";
    }

    if (check2) {
        std::cout << "Matrix subtraction is correct\nTime elapsed: " <<
            subtraction_duration << " seconds\n";
    } else {
        std::cout << "Matrix subtraction is not correct\nTime elapsed: " <<
            subtraction_duration << " seconds\n";
    }

    matrixA = matrixA_ini;
    matrixB = matrixB_ini;

    std::cout << "\n\nMulti threading multiplication: \n";

    start = high_resolution_clock::now();
    multi_matrix_threads(matrixB, rows, k);
    end = high_resolution_clock::now();
    auto multi_threading_duration = duration_cast<nanoseconds>(end - start).count()*1e-9;

    start = high_resolution_clock::now();
    sub_matrix_threads(matrixA, matrixB, rows);
    end = high_resolution_clock::now();
    auto sub_threading_duration = duration_cast<nanoseconds>(end - start).count()*1e-9;

    if (check(matrixB_ini, matrixB, k)) {
        std::cout << "Matrix multiplied by " << k << " is correct \n\tTime elapsed: " <<
            multi_threading_duration << " seconds\n";
    } else {
        std::cout << "Matrix multiplied by " << k << " is not correct \n\tTime elapsed: " <<
            multi_threading_duration << " seconds\n";
    }
    if (final_check(matrixA_ini, matrixB, matrixA)) {
        std::cout << "Matrix substraction is correct \n\tTime elapsed: " <<
            sub_threading_duration << " seconds\n";
    } else {
        std::cout << "Matrix substraction is not correct \n\tTime elapsed: " <<
            sub_threading_duration << " seconds\n";
    }
    return 0;
}