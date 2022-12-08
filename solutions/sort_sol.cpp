#include <algorithm>
#include <functional>
#include <thread>
#include <iostream>
#include <execution>

template<typename T>
void naive_sort(T* A, const std::size_t& sz)
{
    std::sort(A, A+sz);
}

template<typename T>
void std_sort(T* A, const std::size_t& sz)
{
    // std::sort(std::execution::seq, A, A+sz);
    // std::sort(std::execution::unseq, A, A+sz);
    // std::sort(std::execution::par, A, A+sz);
    std::sort(std::execution::par_unseq, A, A+sz);
}

int main()
{
    // int N=8192, *A;
    int N=819200, *A;
    A = (int*)malloc(sizeof(int)*N);
    for (int i = 0; i < N; ++i) {
        A[i] = N-i;
    }
    auto start = std::chrono::high_resolution_clock::now();
    // naive_sort(A, N);
    std_sort(A, N);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end-start;
    std::cout<<elapsed.count() << " ms" <<std::endl;
    std::cout<<A[0] << " " << A[N-1] << std::endl;
}