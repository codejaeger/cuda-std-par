#include <algorithm>
#include <functional>
#include <numeric>
#include <array>
#include <thread>
#include <iostream>
#include <execution>
#include <mutex>
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
    *dummy = std::inner_product(
        A, 
        A+sz, 
        B, 
        *dummy,
        std::plus<>(), 
        [=] (auto &a, auto &b) 
        {
            T tmp = 0;
            for (int k=0; k<WORKLOAD_COUNTER; k++)
                tmp += a*b + sqrt(k);
            return tmp;
        }
        );
}

template<typename T>
void std_transform_reduce(T* A, T* B, const std::size_t& sz, int* dummy)
{
    *dummy = std::transform_reduce(
        // std::execution::unseq, 
        std::execution::par_unseq, 
        A, 
        A+sz, 
        B,
        *dummy,
        std::plus<>(), 
        // std::multiplies<>()
        [=] (auto &a, auto &b) 
        {
            T tmp = 0;
            for (int k=0; k<WORKLOAD_COUNTER; k++)
                tmp += a*b + sqrt(k);
            return tmp;
        }
    );
}


int main()
{
    // int N=8192, *A, *B;
    int N=819200, *A, *B;
    A = (int*)malloc(sizeof(int)*N);
    B = (int*)malloc(sizeof(int)*N);
    
    // This is the culprit.
    for (int i = 0; i < N; ++i) {
        A[i] = 1;
        B[i] = 2;
    }
    int* dummy = new int(0);
    
    auto start = std::chrono::high_resolution_clock::now();
    naive_innerp(A, B, N, dummy);
    std::cout<<*dummy<<std::endl;
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end-start;
    std::cout<<elapsed.count() << " ms" <<std::endl;

    *dummy = 0;
    start = std::chrono::high_resolution_clock::now();
    std_innerp(A, B, N, dummy);
    std::cout<<*dummy<<std::endl;
    end = std::chrono::high_resolution_clock::now();
    elapsed = end-start;
    std::cout<<elapsed.count() << " ms" <<std::endl;

    *dummy = 0;
    start = std::chrono::high_resolution_clock::now();
    std_transform_reduce(A, B, N, dummy);
    std::cout<<*dummy<<std::endl;
    end = std::chrono::high_resolution_clock::now();
    elapsed = end-start;
    std::cout<<elapsed.count() << " ms" <<std::endl;

    // GPU implementation performs good only when you increase the 
    // matrix size or the workload counter (i.e. increase the compute workload)
    // and (optionally) decrease the memory transfers by commenting other kernels and
    // matrix initialisation code.
}