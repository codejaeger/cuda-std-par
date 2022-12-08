#include <iostream>
#include <algorithm>
#include <iterator>

#include <numeric>
#include <utility>
#include <functional>
#include <execution>
#include <thread>

template<typename T>
void matmul_foreach_element(const int& m, const int& n, const int& k, 
                  T* A,
                  T* B,
                  T* C)
{
}

int main() {
    
    // Sample workloads.
    // int m=2048, n=2048, k=2048;
    // int m=512, n=512, k=512;
    // int m=256, n=256, k=256;
    // int m=128, n=128, k=128;

    // This will be the default workload for all benchmarking unless explicitly mentioned in
    // the question.
    int m=1024, n=1024, k=1024;

    int *A, *B, *C;
    A = (int*)malloc(sizeof(int)*m*k); 
    B = (int*)malloc(sizeof(int)*k*n); 
    C = (int*)malloc(sizeof(int)*m*n); 

    // initialize matrix A
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < k; ++j) {
            A[i * k + j] = 1;
        }
    }

    // initialize matrix B
    for (int i = 0; i < k; ++i) {
        for (int j = 0; j < n; ++j) {
            B[i * n + j] = 1;
        }
    }

    auto start = std::chrono::high_resolution_clock::now();

    matmul_foreach_element(m, n, k, A, B, C);
    
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = end-start;

    printf("Time elapsed on matrix multiplication of %dx%d . %dx%d for matmul: %f ms.\n\n", m, k, k, n, elapsed);
    
    std::cout<<"Check " << (C[0]==1024) << "/1" <<std::endl;
    return 0;
}
