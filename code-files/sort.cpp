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

int main()
{
    int N=8192, *A;
    A = (int*)malloc(sizeof(int)*N);
    for (int i = 0; i < N; ++i) {
        A[i] = N-i;
    }


    auto start = std::chrono::high_resolution_clock::now();
    
    // Place your implementation here.
    int runs = 1000;
    for (int ij=0; ij<runs; ij++)
        naive_sort(A, N);
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end-start;
    std::cout<<elapsed.count() / runs << " ms" <<std::endl;
    std::cout<<"Check " << (A[0] == 1) + (A[N-1] == N) << "/2" << std::endl;
}