#include <algorithm>
#include <functional>
#include <numeric>
#include <array>
#include <thread>
#include <iostream>
#include <execution>
#include <mutex>
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
    std::transform(
        std::execution::par_unseq,
        // std::execution::par,
        // std::execution::unseq,
        // std::execution::seq,
        A, 
        A+sz, 
        A,
        [=](int &a_el)
        {
            for (int k=0; k<WORKLOAD_COUNTER; k++)
                a_el += sqrt(k);
            return a_el;
        }
    );
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
    naive_increment(A, N);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end-start;
    std::cout<<A[0]<<std::endl;
    std::cout<<elapsed.count() << " ms" <<std::endl;

    start = std::chrono::high_resolution_clock::now();
    std_transform(B, N);
    end = std::chrono::high_resolution_clock::now();
    elapsed = end-start;
    std::cout<<B[0]<<std::endl;
    std::cout<<elapsed.count() << " ms" <<std::endl;

    // 1. For a workload of 8192 and a low workload counter of 2-20 sequential implementation (on GPU) is far superior.
    // 2. Only when workload counter is increased to 40 GPU is superior.
    //    GPU is superior only when compute intensive work is there
}