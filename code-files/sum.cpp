#include <algorithm>
#include <functional>
#include <numeric>
#include <thread>
#include <iostream>
#include <execution>

template<typename T>
void naive_sum(T* A, const std::size_t& sz, int* dummy)
{
    for (int i=0; i<sz; i++)
        *dummy += A[i];
}

template<typename T>
void std_reduce_sum(T* A, const std::size_t& sz, int* dummy)
{
}

int main()
{
    // int N=8192, *A;
    // int N=819200, *A;
    int N=81920000, *A;
    A = (int*)malloc(sizeof(int)*N);

    for (int i = 0; i < N; ++i) {
        A[i] = 1;
    }
    int* dummy = new int(0);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Place your implementation here instead of naive.
    naive_sum(A, N, dummy);
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end-start;
    std::cout<<elapsed.count() << " ms" <<std::endl;
    std::cout<<"Check " << (*dummy == N) << "/1" << std::endl;
}