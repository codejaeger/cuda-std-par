#include <algorithm>
#include <functional>
#include <numeric>
#include <thread>
#include <iostream>
#include <execution>
#include <cmath>

#define WORKLOAD_COUNTER 5

template<typename T>
void naive_increment(T* A, const std::size_t& sz)
{
    for (int i=0; i<sz; i++)
        for (int k=0; k<WORKLOAD_COUNTER; k++)
            A[i] += sqrt(k);
}

template<typename T>
void std_transform(T* A, const std::size_t& sz)
{
}

int main()
{
    int N=8192, *A, *B;
    // int N=819200, *A, *B;

    A = (int*)malloc(sizeof(int)*N);
    B = (int*)malloc(sizeof(int)*N);
    for (int i = 0; i < N; ++i) {
        A[i] = i;
        B[i] = i;
    }
    
    auto start = std::chrono::high_resolution_clock::now();

    // Place your implementation here instead of naive.
    naive_increment(A, N);
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end-start;
    std::cout<<elapsed.count() << " ms" <<std::endl;
}