#include <iostream>
#include <thread>
#include <atomic>
#include <random>

using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::nanoseconds;

std::atomic<double> execution_duration{0};

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

int main() {
    int rows = 10, cols = 10;
    std::vector<std::vector<int>> matrixA = generate_matrix(rows, cols);
    std::vector<std::vector<int>> matrixB = generate_matrix(rows, cols);
    std::cout << "Initial matrix A:\n";
    print_matrix(matrixA);
    std::cout << "Initial matrix B:\n";
    print_matrix(matrixB);

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
    return 0;
}