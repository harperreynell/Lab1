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

void multiply_matrix(std::vector<std::vector<int>> &matrix, const int k) {
    const int rows = matrix.size();
    const int cols = matrix[0].size();
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] *= k;
        }
    }
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

int main() {
    std::vector<std::vector<int>> matrix = generate_matrix(10, 10);
    std::vector<std::vector<int>> initial = matrix;
    std::cout << "Initial matrix:\n";
    print_matrix(matrix);

    const int k = 10;
    std::cout << "\nMatrix multiplied by " << k << ":\n";
    multiply_matrix(matrix, k);
    print_matrix(matrix);

    std::cout << "\nMatrix check:\n";
    if (check(initial, matrix, k)) {
        std::cout << "Matrix multiplied by " << k << " is correct \n";
    } else {
        std::cout << "Matrix multiplied by " << k << " is NOT correct \n";
    }
    return 0;
}