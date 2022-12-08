#include <iostream>
#include <algorithm>
#include <iterator>

#include <numeric>
#include <utility>
#include <functional>
#include <execution>
#include <thread>

template<typename T>
void matmul_foreach_rowcol(const int& m, const int& n, const int& k, 
                  T* A,
                  T* B,
                  T* C,
                  int* dummy)
{
    std::for_each(
        std::execution::par_unseq,
        // std::execution::par,
        // std::execution::unseq,
        // std::execution::seq,
        thrust::counting_iterator<int>(0), 
        thrust::counting_iterator<int>(m), 
        [=](int r)

        // This was done as an initial finding, not useful now. 
        // Switch to column major by simply changing index access (swap row major and column major).
        // thrust::counting_iterator<int>(n), 
        // [=](int c)
        {
            std::for_each(
                // std::execution::par_unseq,
                // std::execution::par,
                std::execution::unseq,
                // std::execution::seq,
                thrust::counting_iterator<int>(0), 
                thrust::counting_iterator<int>(n), 
                [=](int c)

                // This was done as an initial finding, not useful now. 
                // Switch to column major by simply changing index access.
                // thrust::counting_iterator<int>(m), 
                // [=](int r)
                {
                    T tmp = static_cast<T>(0.0);
                    for (int ik = 0; ik < k; ik++)
                    {
                        // poor performance
                        // C[r*n+c] += A[r*k+ik] * B[ik*n+c];
                        
                        // no sugnificant improvements
                        // C[r*n+c] += A[ik*m+r] * B[ik*n+c];
                        // C[r*n+c] += A[r*k+ik] * B[c*k+ik];

                        // C is the culprit here, since writing is more time consuming
                        // than reading and writing in a column major fashion is better 
                        // than row major.
                        // That is, when the outer loop is parallelised over the rows each 
                        // thread requests an entire row to be read which does not utilise
                        // memory coalescing, however, when the outer loop is over columns
                        // each inner loop requests for elements from an entire column to be read
                        // and they read consecutive elements from the rows simultaneously
                        // which due to memory coalescing performs much better.
                        // This improves performance.
                        // C[c*m+r] += A[r*k+ik] * B[ik*n+c]; 

                        // Use this to verify why the slowdown due to write is caused.
                        // This is perform better than the first version when used with par-unseq outer loop on m
                        // (although this is just a toy example and not matrix multiplication).
                        // *dummy = *dummy + A[r*k+ik] * B[ik*n+c]; 

                        // This should perform the best. A is switched to column major to improve performance.
                        tmp += A[ik*m+r] * B[ik*n+c];
                    }
                  C[c*m+r] = tmp;
                }
            );
        }
    );
}

template<typename T>
void matmul_foreach_rowcol_and_k(const int& m, const int& n, const int& k, 
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
        thrust::counting_iterator<int>(n), 
        [=](int c)
        {
            std::for_each(
                // std::execution::par_unseq,
                // std::execution::par,
                std::execution::unseq,
                // std::execution::seq,
                thrust::counting_iterator<int>(0), 
                thrust::counting_iterator<int>(m), 
                [=](int r)
                {
                    std::for_each(
                        // std::execution::par_unseq,
                        // std::execution::par,
                        std::execution::unseq,
                        // std::execution::seq,
                        A+r*k,
                        A+(r+1)*k,
                        [=](int& a_el)
                        {
                            int k_ind = &a_el - A - r*k;
                            C[r*n+c] += a_el * B[k_ind*n+c];
                        }
                    );
                }
            );
        }
    );
}

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

    int* dummy = new int;
    matmul_foreach_rowcol(m, n, k, A, B, C, dummy);

    // Alternate version - no notable difference in performance
    // matmul_foreach_rowcol_and_k(m, n, k, A, B, C);
    
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = end-start;

    printf("Time elapsed on matrix multiplication of %dx%d . %dx%d for matmul: %f ms.\n\n", m, k, k, n, elapsed);
    
    std::cout<<"Check " << (C[0]==1024) << "/1" <<std::endl;
    return 0;
}
