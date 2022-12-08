#include <algorithm>
#include <functional>
#include <numeric>
#include <thread>
#include <iostream>
#include <execution>
#include <cmath>

#define WORKLOAD_COUNTER 3

template<typename T>
void naive_innerp(T* A, T* B, const std::size_t& sz, int* dummy)
{
    for (int i=0; i<sz; i++)
    {
        for (int k=0; k<WORKLOAD_COUNTER; k++)
            *dummy += A[i] * B[i] + sqrt(k);
    }
}

template<typename T>
void std_innerp(T* A, T* B, const std::size_t& sz, int* dummy)
{
}

template<typename T>
void std_transform_reduce(T* A, T* B, const std::size_t& sz, int* dummy)
{
}


int main()
{
    // int N=8192, *A;
    int N=819200, *A, *B;

    A = (int*)malloc(sizeof(int)*N);
    B = (int*)malloc(sizeof(int)*N);
    
    for (int i = 0; i < N; ++i) {
        A[i] = 1;
        B[i] = 2;
    }
    int* dummy = new int(0);
    
    auto start = std::chrono::high_resolution_clock::now();

    // Place your implementation here instead of naive.
    naive_innerp(A, B, N, dummy);
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end-start;
    std::cout<<elapsed.count() << " ms" <<std::endl;
}
