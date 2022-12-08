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
    std::for_each(
        std::execution::par_unseq,
        // std::execution::par,
        // std::execution::unseq,
        // std::execution::seq,
        thrust::counting_iterator<int>(0), 
        thrust::counting_iterator<int>(m*n), 
        // [=, &A, &B, &C](int index) // illegal memory access
        [=](int index)
        {
            for (int ik = 0; ik < k; ik++)
                C[index] = C[index] + A[int(index/n)*k+ik] * B[ik*n+int(index%n)];
        }
    );
}

template<typename T>
void matmul_foreach_element_and_k(const int& m, const int& n, const int& k, 
                  T* A,
                  T* B,
                  T* C)
{
    std::for_each(
        std::execution::par_unseq,
        // std::execution::par,
        // std::execution::unseq,
        // std::execution::seq,
        thrust::counting_iterator<int>(0), 
        thrust::counting_iterator<int>(m*n), 
        [=](int index)
        {
            std::for_each(
                // std::execution::par_unseq,
                // std::execution::par,
                std::execution::unseq,
                // std::execution::seq,
                thrust::counting_iterator<int>(0), 
                thrust::counting_iterator<int>(k), 
                [=](int ik)
                {
                    C[index] += A[int(index/n)*k+ik] * B[ik*n+int(index%n)];
                }
            );
        }
    );
}

// This function ensures the matrix initialisation happens on the GPU allowing the 
// matrices A and B to be solely on device eliminating unnecessary device to host
// copies.
template <typename T>
void initialize(T* A, const std::size_t& sz, const bool& constant)
{
    std::for_each(
        std::execution::par_unseq,
        A,
        A+sz,
        [=, i=0](int &el) mutable { el = (constant ? 1 : i); /*i++;*/ }
    );
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

    // Memory created on host; would involve unnecessary device to host transfer - comment to improve performance
    // A = (int*)malloc(sizeof(int)*m*k); 
    // B = (int*)malloc(sizeof(int)*k*n); 
    // C = (int*)malloc(sizeof(int)*m*n); 

    // Create memory on device itself memory - uncomment to improve performance
    cudaMalloc((void**)&A, sizeof(int)*m*k);
    cudaMalloc((void**)&B, sizeof(int)*k*n); 
    cudaError_t err = cudaMalloc((void**)&C, sizeof(int)*m*n);

    if( err != cudaSuccess)
	{
		printf("CUDA error: %s\n", cudaGetErrorString(err));
		return -1;
	}

    // Initialisation on the CPU; involves unnecessary memory transfers - comment to improve performance.
    // initialize matrix A
    // for (int i = 0; i < m; ++i) {
    //     for (int j = 0; j < k; ++j) {
    //         A[i * k + j] = 1;
    //     }
    // }
    // initialize matrix B
    // for (int i = 0; i < k; ++i) {
    //     for (int j = 0; j < n; ++j) {
    //         B[i * n + j] = 1;
    //     }
    // }

    // Initialisation on the device - supposed to eliminate unnecessary memcpy cycles
    // uncomment to see performance improvements
    initialize(A, m*k, true);
    initialize(B, k*n, true);

    auto start = std::chrono::high_resolution_clock::now();

    matmul_foreach_element(m, n, k, A, B, C);

    // Alternate version - no notable difference in performance
    // matmul_foreach_element_and_k(m, n, k, A, B, C);
    
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = end-start;

    printf("Time elapsed on matrix multiplication of %dx%d . %dx%d for matmul: %f ms.\n\n", m, k, k, n, elapsed);
    
    // We don't want to cause a device to host memcpy cycle with this extra std::cout
    // Comment out to see improvement.
    // std::cout<<"Check " << (C[0]==1024) << "/1" <<std::endl;
    return 0;
}
