#include <algorithm>
#include <functional>
#include <thread>
#include <iostream>
#include <execution>

template<typename T>
void swaps(T* A, const std::size_t& n)
{
    for(int index=1; index<int(n/2); index++)
    {
        T* ind1 = (A + index - 1);         // Access the previous element.
        T* ind2 = (A + index);             // Access the current element.
        T* ind3 = (A + (index + (n/2))%n); // Access the n/2th element from this index in a circular fashion.
        T tmp = *ind2;
        *ind2 = *ind1;
        *ind1 = *ind3;
        *ind3 = tmp;
    }
}

template<typename T>
void std_swaps(T* A, const std::size_t& n)
{
}

int main()
{
    // int N=8192, *A;
    int N=819200, *A;

    A = (int*)malloc(sizeof(int)*N);
    for (int i = 0; i < N; ++i) {
        A[i] = i;
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Place your implementation here instead of naive.
    swaps(A, N);
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end-start;
    std::cout<<elapsed.count() << " ms" <<std::endl;
}
